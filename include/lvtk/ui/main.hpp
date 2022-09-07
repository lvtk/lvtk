// Copyright 2022 Michael Fisher <mfisher@kushview.net>
// SPDX-License-Identifier: ISC

#pragma once

#include "lvtk/context.hpp"
#include "lvtk/ui/view.hpp"
#include "lvtk/ui/widget.hpp"

namespace lvtk {

/** The running mode type of a UI context. 
    @see Context
 */
enum class Mode {
    PROGRAM = 0,    ///< Standalone application
    MODULE          ///< Loadable plugin or module
};

class Main;

struct Backend {
    Backend() = delete;
    virtual ~Backend() = default;
    const std::string& name() const noexcept { return _name; }
    virtual std::unique_ptr<View> create_view (Main&, Widget&) =0;
protected:
    Backend (const std::string& name)
        : _name (name) {}
private:
    std::string _name;
    Backend (const Backend&) = delete;
    Backend (Backend&&) = delete;
    Backend& operator= (const Backend&) = delete;
    Backend& operator= (Backend&&) = delete;
};

/** The context in which a UI is running.
    Manages Views and the event loop. 
 */
class Main : public Context {
public:
    explicit Main (Mode mode, std::unique_ptr<Backend> backend);
    ~Main();

    /** Returns the running mode of this context. */
    Mode mode() const noexcept { return _mode; }

    /** Update this context. */
    void loop (double timeout);

    /** Request the main loop stop running. */
    void quit();

    /** Create an unrealized view for the given Widget. */
    std::unique_ptr<View> create_view (Widget& widget, uintptr_t parent = 0);

    /** Elevate a Widget to view status */
    void elevate (Widget& widget, uintptr_t parent);
    
    /** Returns the underlying PuglWorld */
    uintptr_t world() const noexcept { return _world; }

    /* things for testing */
    bool __quit_flag = false;
    /* end things for testing */
private:
    uintptr_t _world;
    std::unique_ptr<Backend> _backend;
    const Mode _mode;
    Main() = delete;
    Main (const Main&) = delete;
    Main (Main&&) = delete;
    Main& operator= (const Main&) = delete;
    Main& operator= (Main&&) = delete;
};

}
