/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins-send
 * Created on: 05 авг 2024 г.
 *
 * lsp-plugins-send is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins-send is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins-send. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/plug-fw/meta/ports.h>
#include <lsp-plug.in/shared/meta/developers.h>
#include <private/meta/send.h>

#define LSP_PLUGINS_SEND_VERSION_MAJOR       1
#define LSP_PLUGINS_SEND_VERSION_MINOR       0
#define LSP_PLUGINS_SEND_VERSION_MICRO       0

#define LSP_PLUGINS_SEND_VERSION  \
    LSP_MODULE_VERSION( \
        LSP_PLUGINS_SEND_VERSION_MAJOR, \
        LSP_PLUGINS_SEND_VERSION_MINOR, \
        LSP_PLUGINS_SEND_VERSION_MICRO  \
    )

namespace lsp
{
    namespace meta
    {
        //-------------------------------------------------------------------------
        // Plugin metadata

        // NOTE: Port identifiers should not be longer than 7 characters as it will overflow VST2 parameter name buffers
        static const port_t send_mono_ports[] =
        {
            PORTS_MONO_PLUGIN,

            BYPASS,
            IN_GAIN,
            OUT_GAIN,
            LOG_CONTROL("g_send", "Send gain", U_GAIN_AMP, send::SEND_GAIN),
            SEND_NAME("send", "Audio send connection point name"),
            AUDIO_SEND("sout", "Audio send output", 0, "send"),

            METER_GAIN("ilm", "Input level meter", GAIN_AMP_P_24_DB),
            METER_GAIN("slm", "Send level meter", GAIN_AMP_P_24_DB),
            METER_GAIN("olm", "Output level meter", GAIN_AMP_P_24_DB),


            PORTS_END
        };

        // NOTE: Port identifiers should not be longer than 7 characters as it will overflow VST2 parameter name buffers
        static const port_t send_stereo_ports[] =
        {
            // Input and output audio ports
            PORTS_STEREO_PLUGIN,

            BYPASS,
            IN_GAIN,
            OUT_GAIN,
            LOG_CONTROL("g_send", "Send gain", U_GAIN_AMP, send::SEND_GAIN),
            SEND_NAME("send", "Audio send connection point name"),
            AUDIO_SEND("sout_l", "Audio send output left", 0, "send"),
            AUDIO_SEND("sout_r", "Audio send output right", 1, "send"),

            METER_GAIN("ilm_l", "Input level meter Left", GAIN_AMP_P_24_DB),
            METER_GAIN("slm_l", "Send level meter Left", GAIN_AMP_P_24_DB),
            METER_GAIN("olm_l", "Output level meter Left", GAIN_AMP_P_24_DB),
            METER_GAIN("ilm_r", "Input level meter Right", GAIN_AMP_P_24_DB),
            METER_GAIN("slm_r", "Send level meter Right", GAIN_AMP_P_24_DB),
            METER_GAIN("olm_r", "Output level meter Right", GAIN_AMP_P_24_DB),

            PORTS_END
        };

        static const int plugin_classes[]       = { C_UTILITY, -1 };
        static const int clap_features_mono[]   = { CF_AUDIO_EFFECT, CF_UTILITY, CF_MONO, -1 };
        static const int clap_features_stereo[] = { CF_AUDIO_EFFECT, CF_UTILITY, CF_STEREO, -1 };

        const meta::bundle_t send_bundle =
        {
            "send",
            "Send",
            B_UTILITIES,
            "", // TODO: provide ID of the video on YouTube
            "This plugin allows to perform audio send using the shared memory"
        };

        const plugin_t send_mono =
        {
            "Send Mono",
            "Send Mono",
            "Send Mono",
            "S1M",
            &developers::v_sadovnikov,
            "send_mono",
            {
                LSP_LV2_URI("send_mono"),
                LSP_LV2UI_URI("send_mono"),
                "s01m",
                LSP_VST3_UID("s01m    xxxx"),
                LSP_VST3UI_UID("s01m    xxxx"),
                0,
                NULL,
                LSP_CLAP_URI("send_mono"),
                LSP_GST_UID("send_mono"),
            },
            LSP_PLUGINS_SEND_VERSION,
            plugin_classes,
            clap_features_mono,
            E_DUMP_STATE | E_SHM_TRACKING,
            send_mono_ports,
            "util/send.xml",
            NULL,
            mono_plugin_port_groups,
            &send_bundle
        };

        const plugin_t send_stereo =
        {
            "Send Stereo",
            "Send Stereo",
            "Send Stereo",
            "S1S",
            &developers::v_sadovnikov,
            "send_stereo",
            {
                LSP_LV2_URI("send_stereo"),
                LSP_LV2UI_URI("send_stereo"),
                "s01s",
                LSP_VST3_UID("s01s    yyyy"),
                LSP_VST3UI_UID("s01s    yyyy"),
                0,
                NULL,
                LSP_CLAP_URI("send_stereo"),
                LSP_GST_UID("send_stereo"),
            },
            LSP_PLUGINS_SEND_VERSION,
            plugin_classes,
            clap_features_stereo,
            E_DUMP_STATE | E_SHM_TRACKING,
            send_stereo_ports,
            "util/send.xml",
            NULL,
            stereo_plugin_port_groups,
            &send_bundle
        };
    } /* namespace meta */
} /* namespace lsp */



