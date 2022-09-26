// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <memory>
#include <vector>

#include <lvtk/context.hpp>
#include <lvtk/lvtk.h>
#include <lvtk/ui/view.hpp>

namespace lvtk {
namespace detail {
/** @private */
class Main;
/** @private */
class View;
/** @private */
class Widget;
}

/** The running mode type of a UI context. 
    @see Main
    @ingroup widgets
    @headerfile lvtk/ui/main.hpp
 */
enum class Mode {
    PROGRAM = 0, ///< Standalone application
    MODULE       ///< Loadable plugin or module
};

struct Backend;
class Style;
class View;
class Widget;

/** The context in which a UI or GUI app is running.
    Can create views and run the event loop
    @ingroup widgets
    @headerfile lvtk/ui/main.hpp
 */
class LVTK_API Main : public Context {
public:
    /** Create a main object.
     
        This is the central event controller for GUIs.  It's job is to
        elevate widgets to View status and keep track of events and
        actions.

        @param mode The running mode of the GUI
        @param backend The backend to use for graphics
     */
    explicit Main (Mode mode, std::unique_ptr<Backend> backend);
    ~Main();

    /** Returns the running mode of this context. */
    Mode mode() const noexcept;

    /** Update this context. */
    void loop (double timeout);

    /** Request the main loop stop running. */
    void quit();

    /** Elevate a Widget to view with optional opaque parent */
    void elevate (Widget& widget, ViewFlags flags, uintptr_t parent);

    /** Elevate a Widget to view status with optional known parent */
    void elevate (Widget& widget, ViewFlags flags, View& parent);

    /** Find the view for this wiget */
    View* find_view (Widget& widget) const noexcept;

    /** Returns the OS System Object.
        X11: Returns a pointer to the `Display`.
        MacOS: Returns a pointer to the `NSApplication`.
        Windows: Returns the `HMODULE` of the calling process.
    */
    void* handle() const noexcept;

    /** Returns the underlying PuglWorld. */
    uintptr_t world() const noexcept;

    /** Returns the default style */
    Style& style() noexcept;
    const Style& style() const noexcept;

    /* things for testing */
    bool __quit_flag = false;
    /* end things for testing */

private:
    friend class Widget;
    friend class detail::Widget;
    friend class View;
    friend class detail::View;

    std::unique_ptr<detail::Main> impl;
    LVTK_DISABLE_COPY (Main)
};

/** Backend implementation.
    @headerfile lvtk/ui/main.hpp
    @ingroup widgets
 */
struct Backend {
    Backend()          = delete;
    virtual ~Backend() = default;
    const std::string& name() const noexcept { return _name; }

    /** Your subclass must implement a view and return new
        ones from this factory fuction
    
        @param main     The main object creating the view
        @param widget   The widget being elevated
        @see OpenGL
    */
    virtual std::unique_ptr<View> create_view (Main& main, Widget& widget) = 0;

protected:
    Backend (const std::string& name)
        : _name (name) {}

private:
    std::string _name;
    LVTK_DISABLE_COPY(Backend)
};

} // namespace lvtk
