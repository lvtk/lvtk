/*
 * ui_features.hpp
 *
 *  Created on: Dec 22, 2012
 *      Author: mfisher
 */

#ifndef UI_FEATURES_HPP_
#define UI_FEATURES_HPP_

namespace lvtk {


    /** The URID Mixin.
        @headerfile lvtk/ext/urid.hpp
        @ingroup pluginmixins
        @ingroup guimixins
        @see The internal struct I for details.
     */
    template<bool Required = true>
    struct UIResize
    {
        template<class Derived>
        struct I : Extension<Required>
        {
            I() : p_resize(0) { }

            /** @internal */
            static void
            map_feature_handlers(feature_handler_map& hmap)
            {
                hmap[LV2_UI__resize] = &I<Derived>::handle_feature;
            }

            /** @internal */
            static void
            handle_feature(void* instance, void* data)
            {
                Derived* d = reinterpret_cast<Derived*>(instance);
                I<Derived>* mixin = static_cast<I<Derived>*>(d);

                mixin->p_resize = reinterpret_cast<LV2UI_Resize*>(data);
                mixin->m_ok = true;
            }

            bool
            check_ok()
            {
                if (LVTK_DEBUG)
                {
                    std::clog << "    [UI::Resize] Validation "
                            << (this->m_ok ? "succeeded" : "failed")
                            << "." << std::endl;
                }
                return this->m_ok;
            }

        protected:

            /** */

            bool
            ui_resize(int width, int height)
            {
                return (0 == p_resize->ui_resize (p_resize->handle, width, height));
            }

            LV2UI_Resize* p_resize;

        };
    };

    }

#endif /* UI_FEATURES_HPP_ */