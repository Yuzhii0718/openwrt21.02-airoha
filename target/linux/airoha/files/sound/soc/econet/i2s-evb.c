/*
 * i2s-evb.c  --  I2S machine driver
 *
 * Copyright (c) 2020 Econet Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/of_gpio.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include "i2s-afe-common.h"

struct i2s_evb_priv {
	struct device_node *afe_plat_node;
};

enum {
	/* FE */
	DAI_LINK_FE_BASE = 0,
	DAI_LINK_FE_AFE_BASE = DAI_LINK_FE_BASE,
	DAI_LINK_FE_DL1_PLAYBACK = DAI_LINK_FE_AFE_BASE,
	DAI_LINK_FE_UL1_CAPTURE,
	DAI_LINK_FE_DL2_PLAYBACK,
	DAI_LINK_FE_DL3_PLAYBACK,
	DAI_LINK_FE_DL6_PLAYBACK,
	DAI_LINK_FE_DL8_PLAYBACK,
	DAI_LINK_FE_UL2_CAPTURE,
	DAI_LINK_FE_UL3_CAPTURE,
	DAI_LINK_FE_UL4_CAPTURE,
	DAI_LINK_FE_UL5_CAPTURE,
	DAI_LINK_FE_UL8_CAPTURE,
	DAI_LINK_FE_UL9_CAPTURE,
	DAI_LINK_FE_UL10_CAPTURE,
	DAI_LINK_FE_DL7_PLAYBACK,
	DAI_LINK_FE_AFE_END,
#ifdef CONFIG_SND_SOC_MT8570
	DAI_LINK_FE_SPI_BASE = DAI_LINK_FE_AFE_END,
	DAI_LINK_FE_VA_HOSTLESS = DAI_LINK_FE_SPI_BASE,
	DAI_LINK_FE_SPI_MIC_CAPTURE,
	DAI_LINK_FE_VA_UPLOAD,
	DAI_LINK_SPI_RESERVE2,
	DAI_LINK_SPI_RESERVE3,
	DAI_LINK_SPI_RESERVE4,
	DAI_LINK_FE_SPI_PCMP1,
	DAI_LINK_SPI_RESERVE6,
	DAI_LINK_SPI_RESERVE7,
	DAI_LINK_FE_SPI_LINEIN_CAPTURE,
	DAI_LINK_FE_COMPR_BASE,
	DAI_LINK_FE_COMPRP1 = DAI_LINK_FE_COMPR_BASE,
	DAI_LINK_FE_COMPRP2,
	DAI_LINK_FE_COMPRP3,
	DAI_LINK_FE_A2DP_HOSTLESS,
	DAI_LINK_FE_COMPR_END,
	DAI_LINK_FE_SPI_END = DAI_LINK_FE_COMPR_END,
	DAI_LINK_FE_END = DAI_LINK_FE_SPI_END,
#else
	DAI_LINK_FE_END = DAI_LINK_FE_AFE_END,
#endif
	/* BE */
	DAI_LINK_BE_BASE = DAI_LINK_FE_END,
	DAI_LINK_BE_AFE_BASE = DAI_LINK_BE_BASE,
	DAI_LINK_BE_ETDM1_OUT = DAI_LINK_BE_AFE_BASE,
	DAI_LINK_BE_ETDM1_IN,
	DAI_LINK_BE_ETDM2_OUT,
	DAI_LINK_BE_ETDM2_IN,
	DAI_LINK_BE_PCM_INTF,
	DAI_LINK_BE_VIRTUAL_DL_SOURCE,
	DAI_LINK_BE_DMIC,
	DAI_LINK_BE_INT_ADDA,
	DAI_LINK_BE_GASRC0,
	DAI_LINK_BE_GASRC1,
	DAI_LINK_BE_GASRC2,
	DAI_LINK_BE_GASRC3,
	DAI_LINK_BE_SPDIF_OUT,
	DAI_LINK_BE_SPDIF_IN,
	DAI_LINK_BE_MULTI_IN,
	DAI_LINK_BE_AFE_END,
#ifdef CONFIG_SND_SOC_MT8570
	DAI_LINK_BE_SPI_BASE = DAI_LINK_BE_AFE_END,
	DAI_LINK_BE_SPI_MIC = DAI_LINK_BE_SPI_BASE,
	DAI_LINK_BE_SPI_PRIMARY_PLAYBACK,
	DAI_LINK_BE_SPI_LINEIN,
	DAI_LINK_BE_SPI_END,
	DAI_LINK_BE_END = DAI_LINK_BE_SPI_END,
#else
	DAI_LINK_BE_END = DAI_LINK_BE_AFE_END,
#endif
	DAI_LINK_NUM = DAI_LINK_BE_END,
	DAI_LINK_FE_NUM = DAI_LINK_FE_END - DAI_LINK_FE_BASE,
	DAI_LINK_BE_NUM = DAI_LINK_BE_END - DAI_LINK_BE_BASE,
};

/* Digital audio interface glue - connects codec <---> CPU */
static struct snd_soc_dai_link i2s_evb_dais[] = {
	[DAI_LINK_FE_DL1_PLAYBACK] = {
		.name = "DL1",
		.stream_name = "DL1 Playback",
		.cpu_dai_name = "DL1",
		.platform_name = "i2s-afe-pcm",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 0,
		.dpcm_playback = 1,
	},
	[DAI_LINK_FE_UL1_CAPTURE] = {
		.name = "UL1",
		.stream_name = "UL1 Record",
		.cpu_dai_name = "UL1",
		.platform_name = "i2s-afe-pcm",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 0,
		.dpcm_capture = 1,
	},
};

static const struct snd_kcontrol_new i2s_evb_controls[] = {
#ifdef CONFIG_SND_SOC_GAPP_AUDIO_CONTROL
	SOC_SINGLE_EXT("Master Volume 1", 0, 0, 100, 0,
		       soc_ctlx_get, soc_ctlx_put),
	SOC_SINGLE_EXT("Master Volume X", 0, 0, 100, 0,
		       soc_ctlx_get, soc_ctlx_put),
	SOC_SINGLE_BOOL_EXT("Master Switch", 0,
			    soc_ctlx_get, soc_ctlx_put),
	SOC_SINGLE_BOOL_EXT("Master Switch X", 0,
			    soc_ctlx_get, soc_ctlx_put),
	SOC_ENUM_EXT("PCM State", pcm_state_enums,
		     soc_pcm_state_get, soc_pcm_state_put),
	SOC_ENUM_EXT("PCM State X", pcm_state_enums,
		     soc_pcm_state_get, 0),
#endif
};

static struct snd_soc_card i2s_evb_card = {
	.name = "i2s-evb-card",
	.owner = THIS_MODULE,
	.dai_link = i2s_evb_dais,
	.num_links = ARRAY_SIZE(i2s_evb_dais),
};


static void i2s_evb_cleanup_of_resource(struct snd_soc_card *card)
{
	struct i2s_evb_priv *priv = snd_soc_card_get_drvdata(card);
	struct snd_soc_dai_link *dai_link;
	int i, j;

	of_node_put(priv->afe_plat_node);


	for (i = 0, dai_link = card->dai_link;
	     i < card->num_links; i++, dai_link++) {
		if (dai_link->num_codecs > 1) {
			struct snd_soc_dai_link_component *codec;

			for (j = 0, codec = dai_link->codecs;
			     j < dai_link->num_codecs; j++, codec++) {
				if (!codec)
					break;
				of_node_put(codec->of_node);
			}
		} else if (dai_link->num_codecs == 1)
			of_node_put(dai_link->codec_of_node);
	}
}


static int i2s_evb_dev_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card = &i2s_evb_card;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *afe_plat_node;
#ifdef CONFIG_SND_SOC_MT8570
	struct device_node *spi_plat_node;
#endif
	struct i2s_evb_priv *priv;
	int ret, id;
	size_t i;
	size_t dais_num = ARRAY_SIZE(i2s_evb_dais);

	for (i = 0; i < dais_num; i++) {
		if (i2s_evb_dais[i].platform_name)
			continue;
		i2s_evb_dais[i].platform_of_node = afe_plat_node;
	}

	card->dev = dev;

	priv = devm_kzalloc(dev, sizeof(struct i2s_evb_priv),
			    GFP_KERNEL);
	if (!priv) {
		ret = -ENOMEM;
		dev_err(dev, "%s allocate card private data fail %d\n",
			__func__, ret);
		return ret;
	}

	priv->afe_plat_node = afe_plat_node;

	snd_soc_card_set_drvdata(card, priv);

	ret = devm_snd_soc_register_card(dev, card);
	if (ret) {
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "%s snd_soc_register_card fail %d\n",
				__func__, ret);
		return ret;
	}

	dev_info(dev, "%s done\n", __func__);

	return ret;
}

static int i2s_evb_dev_remove(struct platform_device *pdev)
{
	struct snd_soc_card *card = platform_get_drvdata(pdev);

	i2s_evb_cleanup_of_resource(card);

	return 0;
}

static const struct of_device_id i2s_evb_dt_match[] = {
	{ .compatible = "i2s,i2s-evb", },
	{ }
};
MODULE_DEVICE_TABLE(of, i2s_evb_dt_match);

static struct platform_driver i2s_evb_driver = {
	.driver = {
		   .name = "i2s-evb",
		   .of_match_table = i2s_evb_dt_match,
	},
	.probe = i2s_evb_dev_probe,
	.remove = i2s_evb_dev_remove,
};

module_platform_driver(i2s_evb_driver);

/* Module information */
MODULE_DESCRIPTION("I2S EVB SoC machine driver");
MODULE_AUTHOR("I2S");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:i2s-evb");

