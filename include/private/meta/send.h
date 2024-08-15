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

#ifndef PRIVATE_META_SEND_H_
#define PRIVATE_META_SEND_H_

#include <lsp-plug.in/plug-fw/meta/types.h>
#include <lsp-plug.in/plug-fw/const.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Plugin metadata
    namespace meta
    {
        typedef struct send
        {
            static constexpr float  SEND_GAIN_MIN       = GAIN_AMP_M_INF_DB;
            static constexpr float  SEND_GAIN_MAX       = GAIN_AMP_P_24_DB;
            static constexpr float  SEND_GAIN_DFL       = GAIN_AMP_0_DB;
            static constexpr float  SEND_GAIN_STEP      = 0.01f;
        } send;

        // Plugin type metadata
        extern const plugin_t send_mono;
        extern const plugin_t send_stereo;

    } /* namespace meta */
} /* namespace lsp */

#endif /* PRIVATE_META_SEND_H_ */
