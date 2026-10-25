#include "precomp.h"

/* Variables */
struct MULTI_PROC_OPS ipc_sock_ops = {
	.multi_proc_init = multi_proc_init,
	.multi_insert_q = ipc_sock_insert_q,
	.multi_lock_q = ipc_sock_lock_q,
	.multi_unlock_q = ipc_sock_unlock_q,
	.multi_wait_data = ipc_sock_wait_data,
	.multi_sig_data = ipc_sock_sig_data,
	.multi_proc_close = ipc_sock_close,
};
#define ADDRESS "ATED_SOCK"

/* Prive Function Prototype */
/* Functions */
int multi_proc_init(void **dri_if, int idx, int dri_if_num, void *pkt_proc_logic)
{
	struct DRI_IF **a_ifs = (struct DRI_IF **)dri_if;
	struct DRI_IF *p_if = a_ifs[idx];
	struct MULTI_PROC_SOCK *priv_data = NULL;
	void (*handler)(void *arg) = pkt_proc_logic;
	int pid;
	int i;

	priv_data = (struct MULTI_PROC_SOCK *)malloc(sizeof(struct MULTI_PROC_SOCK));
	ate_printf(MSG_INFO, "%d, malloc priv_data\n", getpid());

	if (!priv_data)
		return -1;

	p_if->priv_data = priv_data;
	/* Open PIPE */
	if (pipe(priv_data->fd))
		return -1;
	/* Init Queue */
	priv_data->q.served = 0;
	priv_data->q.un_served = 0;

	for (i = 0; i < CMD_QUEUE_SIZE; i++) {
		priv_data->q.cmd_len[i] = -1;
		os_memset(&priv_data->q.cmd_arr[i], 0, PKT_BUF_SIZE);
	}

	pid = fork();

	switch (pid) {
	case -1:
		perror("fork");
		exit(1);

	case 0:
		priv_data->pid = getpid();
		close(priv_data->fd[PIPE_WRITE]);
#ifdef MT_ATED_TCP_SUPPORT
		close_eth_tcp_init_sock();
#endif
		break;

	default:
		priv_data->pid = getpid();
		close(priv_data->fd[PIPE_READ]);
		return 0;
	}

	if (pkt_proc_logic != NULL)
		handler(p_if);

	return 0;
}

/* parent process handle */
int ipc_sock_insert_q(void *dri_if, unsigned char *pkt, int len)
{
	struct DRI_IF *p_if = dri_if;
	struct MULTI_PROC_SOCK *priv_data = (struct MULTI_PROC_SOCK *)p_if->priv_data;
	int out;
	struct cmd_queue *q = &priv_data->q;
	int *served = &q->served;
	int ret = 0;

	out = priv_data->fd[PIPE_WRITE];
	ate_printf(MSG_DEBUG, "%d, pid[%d]PIPE write %d data\n", __LINE__, priv_data->pid, len);

	ret = write(out, pkt, len);

	if (ret < 0)
		ate_printf(MSG_ERROR, "%d, pid[%d]PIPE write ERR %d\n", __LINE__, priv_data->pid, ret);

	*served = (*served + 1) % CMD_QUEUE_SIZE;

	return ret;
}

/* child process handle */
int ipc_sock_wait_data(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_SOCK *priv_data = p_if->priv_data;
	struct cmd_queue *q = &priv_data->q;
	int fd = priv_data->fd[PIPE_READ];
	int *un_served = &q->un_served;
	int count = 0;
	unsigned char buf[PKT_BUF_SIZE];
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd, &set);

	count = select(FD_SETSIZE, &set, NULL, NULL, NULL);

	if (count == 0)
		ate_printf(MSG_ERROR, "IPC_SOCK_WAIT_DATA: Nothing\n");
	else if (count < 0)
		ate_printf(MSG_ERROR, "IPC_SOCK_WAIT_DATA: Error\n");
	else {
		count = read(fd, buf, PKT_BUF_SIZE);

		if (count < 0) {
			if (errno != EINTR) {
				ate_printf(MSG_ERROR, "PIPE Read Error, errno: 0x%x\n", errno);
				/* exit(0); */
			} else
				ate_printf(MSG_ERROR, "PIPE Read INTERRUPTED, errno: 0x%x\n", errno);
		} else if (count == 0) {
			perror("ipc_sock_wait_data:");
			printf("%s %d, count:%d\n",  __func__, __LINE__, count);

			ate_printf(MSG_ERROR, "PIPE Read INTERRUPTED, 0  errno: 0x%x\n", errno);
			return 0;
		} else if ((q->cmd_len[*un_served]) != -1) {
			ate_printf(MSG_ERROR, "CMD Q is full\n");
			return -1;
		}

		ate_printf(MSG_INFO, "[%d]PIPE Read %d bytes\n", getpid(), count);

		os_memcpy(q->cmd_arr[*un_served], buf, PKT_BUF_SIZE);
		q->cmd_len[*un_served] = count;
		*un_served = (*un_served + 1) % CMD_QUEUE_SIZE;

		ate_printf(MSG_INFO, "[%d]*un_served: %d\n", getpid(), *un_served);
	}

	return count;
}

int ipc_sock_lock_q(void *dri_if)
{
	return 0;
}

int ipc_sock_unlock_q(void *dri_if)
{
	return 0;
}

int ipc_sock_sig_data(void *dri_if)
{
	return 0;
}

int ipc_sock_close(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_SOCK *priv_data = p_if->priv_data;
	int pid = 0;
	int my_pid = getpid();
	int *fd = NULL;
	int status;
	int ret = 0;

	if (priv_data) {
		ate_printf(MSG_INFO, "%d, free priv_data\n", getpid());
		pid = priv_data->pid;
		fd = priv_data->fd;
		close(fd[PIPE_READ]);
		close(fd[PIPE_WRITE]);
		free(priv_data);
	}

	if (my_pid == pid) {
		ate_printf(MSG_INFO, "Forked sub-process is terminating at (%s)\n", p_if->ifname);
		exit(0);
	} else {
		kill(pid, SIGTERM);
		ate_printf(MSG_INFO, "%d, wait child(%d)\n", getpid(), pid);
		waitpid(pid , &status, 0);
		ate_printf(MSG_INFO, "%d, child finish\n", getpid());
	}

	return ret;
}
