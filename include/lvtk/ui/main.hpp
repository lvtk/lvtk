// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <lvtk/lvtk.h>

#include <lvtk/context.hpp>
#include <lvtk/ui/view.hpp>

namespace lvtk {
struct Backend;
class Style;
class Widget;
namespace detail {
class Main;
class View;
class Widget;
} // namespace detail

/** The running mode type of a UI context. 
    @see Main
    @ingroup widgets
    @headerfile lvtk/ui/main.hpp
 */
enum class Mode {
    PROGRAM = 0, ///< Standalone application
    MODULE       ///< Loadable plugin or module
};

/** The context in which a UI or GUI app is running.
    Can create views and run the event loop
    @ingroup widgets
    @headerfile lvtk/ui/main.hpp
 */
class LVTK_API Main : public Context {
public:
    enum Status : int {
        RUNNING = 0,
        QUIT_REQUESTED
    };

    /** Create a main object.
     
        This is the central event controller for GUIs.  It's job is to
        elevate widgets to View status and keep track of events and
        actions.

        @param mode The running mode of the GUI
        @param backend The backend to use for graphics
     */
    explicit Main (Mode mode, std::unique_ptr<Backend> backend);
    ~Main();

    /** Returns the running mode of this context.
        @returns Mode
    */
    Mode mode() const noexcept;

    /** Run the event loop this context.
        
        Update by processing events from the window system.

        This function is a single iteration of the main loop, and should be called
        repeatedly to update all views.

        If `timeout` is zero, then this function will not block.  Plugins should
        always use a timeout of zero to avoid blocking the host.

        If a positive `timeout` is given, then events will be processed for that
        amount of time, starting from when this function was called.

        If a negative `timeout` is given, this function will block indefinitely
        until an event occurs.

        For continuously animating programs, a timeout that is a reasonable fraction
        of the ideal frame period should be used, to minimize input latency by
        ensuring that as many input events are consumed as possible before drawing.

        @param timeout Timeout in seconds
    */
    void loop (double timeout);

    /** True if the loop is running in one way or the other.
        @note that this will return false before the first call to Main::loop
        @returns bool True if running.
    */
    bool running() const noexcept;

    /** Returns the exit code currently set
        @returns in The exit code
    */
    int exit_code() const noexcept;

    /** Set the current exit code
        Internally exit code does not get set. It is up to the User to change it
        from the default, zero, to what you need
    */
    void set_exit_code (int code);

    /** Request the main loop stop running. */
    void quit();

    /** Elevate a Widget to view with optional opaque parent */
    void elevate (Widget& widget, ViewFlags flags, uintptr_t parent);

    /** Elevate a Widget to view status with optional known parent */
    void elevate (Widget& widget, ViewFlags flags, View& parent);

    /** Find the view for this wiget */
    View* find_view (Widget& widget) const noexcept;

    /** Returns the default style
        @returns Style
     */
    Style& style() noexcept;

    /** Returns the default style (const)
        @returns Style
     */
    const Style& style() const noexcept;

    /** Returns the OS System Object.
        X11: Returns a pointer to the `Display`.
        MacOS: Returns a pointer to the `NSApplication`.
        Windows: Returns the `HMODULE` of the calling process.
    */
    void* handle() const noexcept;

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
    LVTK_DISABLE_COPY (Backend)
};

} // namespace lvtk
