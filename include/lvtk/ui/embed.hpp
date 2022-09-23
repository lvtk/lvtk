// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/lvtk.h>
#include <lvtk/ui/widget.hpp>

namespace lvtk {

class Main;

/** Embed native views in another Widget. 
    @ingroup widgets
    @headerfile lvtk/ui/embed.hpp
 */
class LVTK_API Embed final : public Widget {
public:
    /** Initialize an embeddable. */
    Embed (Main& main);
    ~Embed();

    /** Returns the view of the embedded object */
    ViewRef host_view() const noexcept;

protected:
    /** @internal */
    void paint (Graphics& g) override;
    void resized() override;
    void children_changed() override;
    void parent_structure_changed() override;

private:
    class Window;
    std::unique_ptr<Window> window;
    LVTK_DISABLE_COPY (Embed);
};

} // namespace lvtk
