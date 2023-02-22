// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/widget.hpp>

namespace lvtk {

namespace detail {
/** @private */
class Embed;
} // namespace detail

class Main;

/** Embed native views in another Widget. 
    @ingroup widgets
    @headerfile lvtk/ui/embed.hpp
 */
class LVTK_API Embed final : public Widget {
public:
    /** Initialize an embeddable widget */
    Embed();
    ~Embed();

    /** Returns the view of the embedded object
        @returns the host view of the proxy.
     */
    ViewRef proxy_view() const noexcept;

    /** Attach a native window handle. 
        To detach pass nullptr.
    */
    void attach (uintptr_t native);

protected:
    /** @internal */
    void paint (Graphics& g) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void children_changed() override;
    /** @internal */
    void parent_structure_changed() override;

private:
    std::unique_ptr<detail::Embed> impl;
    LVTK_DISABLE_COPY (Embed)
};

} // namespace lvtk
