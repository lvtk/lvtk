// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <memory>
#include <vector>

#include <lvtk/context.hpp>
#include <lvtk/lvtk.h>
#include <lvtk/string.hpp>
#include <lvtk/ui/view.hpp>

namespace lvtk {
namespace detail {
/** @private */
class Main;
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
    Mode mode() const noexcept { return _mode; }

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
    uintptr_t world() const noexcept { return _world; }

    /** Returns the default style */
    Style& style() noexcept { return *_style; }
    const Style& style() const noexcept { return *_style; }

    /* things for testing */
    bool __quit_flag = false;
    /* end things for testing */

private:
    friend class Widget;
    friend class View;
    uintptr_t _world;
    std::unique_ptr<Backend> _backend;
    const Mode _mode;
    std::vector<View*> _views;
    std::unique_ptr<Style> _style;

    /** Create an unrealized view for the given Widget. */
    std::unique_ptr<View> create_view (Widget& widget, ViewFlags flags, uintptr_t parent);

    Main()             = delete;
    Main (const Main&) = delete;
    Main (Main&&)      = delete;
    Main& operator= (const Main&) = delete;
    Main& operator= (Main&&) = delete;
};

/** Backend implementation.
    @headerfile lvtk/ui/main.hpp
    @ingroup widgets
 */
struct Backend {
    Backend()          = delete;
    virtual ~Backend() = default;
    const String& name() const noexcept { return _name; }

    /** Your subclass must implement a view and return new
        ones from this factory fuction
    
        @param main     The main object creating the view
        @param widget   The widget being elevated
        @see OpenGL
    */
    virtual std::unique_ptr<View> create_view (Main& main, Widget& widget) = 0;

protected:
    Backend (const String& name)
        : _name (name) {}

private:
    String _name;
    Backend (const Backend&) = delete;
    Backend (Backend&&)      = delete;
    Backend& operator= (const Backend&) = delete;
    Backend& operator= (Backend&&) = delete;
};

} // namespace lvtk
