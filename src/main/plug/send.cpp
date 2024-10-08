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
#include <lsp-plug.in/plug-fw/core/AudioBuffer.h>
#include <lsp-plug.in/plug-fw/meta/func.h>
#include <lsp-plug.in/shared/debug.h>

#include <private/plugins/send.h>

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
            fOutGain        = GAIN_AMP_M_INF_DB;
            fSendGain       = GAIN_AMP_M_INF_DB;

            pBypass         = NULL;
            pInGain         = NULL;
            pOutGain        = NULL;
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

            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];

                c->sBypass.construct();

                c->pIn              = NULL;
                c->pOut             = NULL;
                c->pSend            = NULL;

                c->pInMeter         = NULL;
                c->pOutMeter        = NULL;
                c->pSendMeter       = NULL;
            }

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
            BIND_PORT(pOutGain);
            BIND_PORT(pSendGain);

            lsp_trace("Binding send ports");
            SKIP_PORT("Send name");
            for (size_t i=0; i<nChannels; ++i)
                BIND_PORT(vChannels[i].pSend);

            lsp_trace("Binding meters");
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];

                BIND_PORT(c->pInMeter);
                BIND_PORT(c->pSendMeter);
                BIND_PORT(c->pOutMeter);
            }
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
                for (size_t i=0; i<nChannels; ++i)
                {
                    channel_t *c        = &vChannels[i];
                    c->sBypass.destroy();
                }

                free(vChannels);
                vChannels       = NULL;
            }
        }

        void send::update_sample_rate(long sr)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];
                c->sBypass.init(sr);
            }
        }

        void send::update_settings()
        {
            const bool bypass       = pBypass->value() >= 0.5f;

            fInGain                 = pInGain->value();
            fOutGain                = pOutGain->value() * fInGain;
            fSendGain               = pSendGain->value() * fInGain;

            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];
                c->sBypass.set_bypass(bypass);
            }
        }

        void send::process(size_t samples)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];
                const float *in     = c->pIn->buffer<float>();
                float *out          = c->pOut->buffer<float>();

                core::AudioBuffer *send_buf = c->pSend->buffer<core::AudioBuffer>();
                float *send         = ((send_buf != NULL) && (send_buf->active())) ? send_buf->buffer() : NULL;

                const float ilm     = dsp::abs_max(in, samples);
                dsp::mul_k3(out, in, fOutGain, samples);
                if (send != NULL)
                    c->sBypass.process_wet(send, NULL, in, fSendGain, samples);

                if (c->pInMeter != NULL)
                    c->pInMeter->set_value(ilm * fInGain);
                if (c->pSendMeter != NULL)
                    c->pSendMeter->set_value(ilm * fSendGain);
                if (c->pOutMeter != NULL)
                    c->pOutMeter->set_value(ilm * fOutGain);
            }
        }

        void send::dump(dspu::IStateDumper *v) const
        {
            plug::Module::dump(v);

            v->write("nChannels", nChannels);
            v->begin_array("vChannels", vChannels, nChannels);
            {
                for (size_t i=0; i<nChannels; ++i)
                {
                    const channel_t *c = &vChannels[i];

                    v->begin_object(c, sizeof(channel_t));
                    {
                        v->write_object("sBypass", &c->sBypass);

                        v->write("pIn", c->pIn);
                        v->write("pOut", c->pOut);
                        v->write("pSend", c->pSend);

                        v->write("pInMeter", c->pInMeter);
                        v->write("pOutMeter", c->pOutMeter);
                        v->write("pSendMeter", c->pSendMeter);
                    }
                    v->end_object();
                }
            }
            v->end_array();

            v->write("fInGain", fInGain);
            v->write("fOutGain", fOutGain);
            v->write("fSendGain", fSendGain);

            v->write("pBypass", pBypass);
            v->write("pInGain", pInGain);
            v->write("pOutGain", pOutGain);
            v->write("pSendGain", pSendGain);
        }

    } /* namespace plugins */
} /* namespace lsp */


