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

#ifndef PRIVATE_PLUGINS_SEND_H_
#define PRIVATE_PLUGINS_SEND_H_

#include <lsp-plug.in/dsp-units/util/Delay.h>
#include <lsp-plug.in/dsp-units/ctl/Bypass.h>
#include <lsp-plug.in/plug-fw/plug.h>
#include <private/meta/send.h>

namespace lsp
{
    namespace plugins
    {
        /**
         * Base class for the latency compensation delay
         */
        class send: public plug::Module
        {
            private:
                typedef struct channel_t
                {
                    dspu::Bypass        sBypass;        // Bypass

                    plug::IPort        *pIn;            // Input port
                    plug::IPort        *pOut;           // Output port
                    plug::IPort        *pSend;          // Send port

                    plug::IPort        *pInMeter;       // Input level meter
                    plug::IPort        *pOutMeter;      // Output level meter
                    plug::IPort        *pSendMeter;     // Send level meter
                } channel_t;

            protected:
                size_t              nChannels;          // Number of channels
                channel_t          *vChannels;          // Channels
                float               fInGain;            // Input gain
                float               fOutGain;           // Output gain
                float               fSendGain;          // Send gain

                plug::IPort        *pBypass;            // Bypass port
                plug::IPort        *pInGain;            // Input gain
                plug::IPort        *pOutGain;           // Output gain
                plug::IPort        *pSendGain;          // Send gain

            protected:
                void                do_destroy();

            public:
                explicit send(const meta::plugin_t *meta);
                send(const send &) = delete;
                send(send &&) = delete;
                virtual ~send() override;

                send & operator = (const send &) = delete;
                send & operator = (send &&) = delete;

                virtual void        init(plug::IWrapper *wrapper, plug::IPort **ports) override;
                virtual void        destroy() override;

            public:
                virtual void        update_sample_rate(long sr) override;
                virtual void        update_settings() override;
                virtual void        process(size_t samples) override;
                virtual void        dump(dspu::IStateDumper *v) const override;
        };

    } /* namespace plugins */
} /* namespace lsp */


#endif /* PRIVATE_PLUGINS_SEND_H_ */

