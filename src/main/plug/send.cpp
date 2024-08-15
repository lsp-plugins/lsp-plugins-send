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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/dsp-units/units.h>
#include <lsp-plug.in/plug-fw/meta/func.h>
#include <lsp-plug.in/shared/debug.h>

#include <private/plugins/send.h>

/* The size of temporary buffer for audio processing */
#define BUFFER_SIZE         0x1000U

namespace lsp
{
    namespace plugins
    {
        //---------------------------------------------------------------------
        // Plugin factory
        static const meta::plugin_t *plugins[] =
        {
            &meta::send_mono,
            &meta::send_stereo
        };

        static plug::Module *plugin_factory(const meta::plugin_t *meta)
        {
            return new send(meta);
        }

        static plug::Factory factory(plugin_factory, plugins, 2);

        //---------------------------------------------------------------------
        // Implementation
        send::send(const meta::plugin_t *meta):
            Module(meta)
        {
            // Compute the number of audio channels by the number of inputs
            nChannels       = 0;

            for (const meta::port_t *p = meta->ports; p->id != NULL; ++p)
                if (meta::is_audio_in_port(p))
                    ++nChannels;

            vChannels       = NULL;
            fInGain         = GAIN_AMP_M_INF_DB;
            fSendGain       = GAIN_AMP_M_INF_DB;

            pBypass         = NULL;
            pInGain         = NULL;
            pSendGain       = NULL;
        }

        send::~send()
        {
            do_destroy();
        }

        void send::init(plug::IWrapper *wrapper, plug::IPort **ports)
        {
            // Call parent class for initialization
            Module::init(wrapper, ports);

            size_t to_alloc     = sizeof(channel_t) * nChannels;
            vChannels           = static_cast<channel_t *>(malloc(to_alloc));
            if (vChannels == NULL)
                return;

            size_t port_id      = 0;

            // Bind inputs and outpus
            lsp_trace("Binding inputs and outputs");
            for (size_t i=0; i<nChannels; ++i)
                BIND_PORT(vChannels[i].pIn);
            for (size_t i=0; i<nChannels; ++i)
                BIND_PORT(vChannels[i].pOut);

            lsp_trace("Binding common ports");
            BIND_PORT(pBypass);
            BIND_PORT(pInGain);
            BIND_PORT(pSendGain);

            lsp_trace("Binding send ports");
            SKIP_PORT("Send name");
            for (size_t i=0; i<nChannels; ++i)
                BIND_PORT(vChannels[i].pSend);
        }

        void send::destroy()
        {
            Module::destroy();
            do_destroy();
        }

        void send::do_destroy()
        {
            if (vChannels != NULL)
            {
                free(vChannels);
                vChannels       = NULL;
            }
        }

        void send::update_settings()
        {
            float send_gain     = (pBypass->value() <= 0.5f) ? pSendGain->value() : 0.0f;

            fInGain             = pInGain->value();
            fSendGain           = send_gain * fInGain;
        }

        void send::process(size_t samples)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];
                const float *in     = c->pIn->buffer<float>();
                float *out          = c->pOut->buffer<float>();
                float *send         = c->pSend->buffer<float>();

                dsp::mul_k3(out, in, fInGain, samples);
                if (send != NULL)
                    dsp::mul_k3(send, in, fSendGain, samples);
            }
        }

        void send::dump(dspu::IStateDumper *v) const
        {
            plug::Module::dump(v);

            // TODO
        }

    } /* namespace plugins */
} /* namespace lsp */


