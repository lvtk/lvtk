###########
Style Guide
###########

Enum Basics
===========

Casing
~~~~~~

Use snake case capital letters.

.. code:: cpp

    // Good
    struct Obj {
        enum Type : int {
            BUFFER = 0
        };
    };

.. code:: cpp

    // Not good
    struct Obj {
        enum Type : int {
            AudioBuffer = 0
        };
    };

Type and Size
~~~~~~~~~~~~~
Always specify type in c++.

.. code:: cpp

    // good
    struct Obj {
        enum Options : uint32_t {
            ENABLED = (1u << 0u)
        };
    };

    // Not good
    struct Obj {
        enum Options {
            ASSUME_COMPILER_PICKS_TYPE_AND_SIZE = (1 << 0)
        };
    };

Enum Aliases
============

Direct alias' of LV2 types. Enums from LV2 should be directly aliased to
avoid constant casting.

.. code:: cpp

    // good because types are identical.
    using WorkerStatus = LV2_Worker_Status;

    // not good because differing types need casted back and forth.
    enum WorkerStatus {
        SUCCESS = LVW_WORKER_SUCCESS
    };

C++ Enum Class
==============

Use when a dumb enumeration is needed and is only used like a KEY in a
key/value relationship or for a switch statement.

-  **Always**: Specify type
-  **Good**: if it's a dumb enum
-  **Good**: if it describes a "type" or "mode" of something
-  **Bad**: nested in a class
-  **Bad**: if it needs a method
-  **Bad**: for flags.

.. code:: cpp

    // good
    enum class Mode {
        PROGRAM = 0, ///< Standalone application
        MODULE       ///< Loadable plugin or module
    };

    // good
    enum class PathOp : int {
        MOVE = 100000, ///< Move to
        LINE,          ///< Line to
        QUADRATIC,     ///< Quad to
        CUBIC,         ///< Cubic to
        CLOSE          ///< Close path
    };

    // not good
    class Object {
        enum class Category {
            COLOR,
            SIZE
            // and so on
        };
    };

*The not good example above works fine. The reason it isn't allowed is
so there is a clear distinction between these and enum struct option
(which is used more often in this library)*

Struct/Class Enums
==================

Use when a struct or class has options or settings pertaining to itself,
the typename is like a "namespace" for the values, or is considered a
"smart" enum with methods attached.

This is the most commonly used format throughout LVTK. Enums of this
type should implement operators for equality, assignment, and copy
construction.

Enum With Methods
~~~~~~~~~~~~~~~~~

.. code:: cpp

    // good
    struct SmartEnum {
        enum Type : uint32_t {
            TYPE_ONE = 1,
            TYPE_TWO = 2
        };

        std::string str() const noexecpt {
            return std::to_string (static_cast<int> (_type));
        }

    private:
        uint32_t _type;
    }

Objects With Flags
~~~~~~~~~~~~~~~~~~

-  **ALWAYS** use unsigned int types.
-  **Define** the SomethingFlags
-  **Uniform** methods if possible - TBD
-  **Try** to use "Flag" as the enum typename

   -  Not to important as long as it's obvious what it is.

.. code:: cpp

    using SomethingFlags = uint32_t;

    // good
    struct Something {
        enum Flag : uint32_t {
            OPT_1 = (1u << 0u),
            OPT_2 = (1u << 1u)
            /// and so on.
        };

        SomethingFlags flags() const noexcept { return _flags; }
    };

Objects As Flag
~~~~~~~~~~~~~~~

Basically the same as *Objects With Flags*, but the object itself is
also the flag. This could be done as ``class enums``, however, doing it
this way doesn't need operator overloading for OR'ing to work.

.. code:: cpp

    class Align final {
    public:
        enum Placement : uint8_t {
            LEFT   = (1u << 0u), ///< Align left.
            CENTER = (1u << 2u), ///< Align horizontally centered.
            RIGHT  = (1u << 1u), ///< Align right.

            TOP    = (1u << 3u), ///< Align to top.
            MIDDLE = (1u << 5u), ///< Align vertically centered.
            BOTTOM = (1u << 4u), ///< Align to bottom.

            /// @brief Align to top-left corner
            TOP_LEFT = TOP | LEFT,
            /// @brief Align center at the top
            TOP_CENTER = TOP | CENTER,
            /// @brief Align to top-right corner
            TOP_RIGHT = TOP | RIGHT,

            /// @brief Aligned left in the middle
            LEFT_MIDDLE = LEFT | MIDDLE,
            /// @brief Align centered vertically and horizontally
            CENTERED = CENTER | MIDDLE,
            /// @brief Aligned right in the middle
            RIGHT_MIDDLE = RIGHT | MIDDLE,

            /// @brief Align to bottom-left corner
            BOTTOM_LEFT = BOTTOM | LEFT,
            /// @brief Align center at the bottom
            BOTTOM_CENTER = BOTTOM | CENTER,
            /// @brief Align to bottom-right corner
            BOTTOM_RIGHT = BOTTOM | RIGHT
        };

        /** Create an invalid alignment */
        Align() {}
        /** Create an alignment from flags */
        Align (uint8_t flags) : _flags (flags) {}
        /** Copy this alignment */
        Align (const Align& o) : _flags (o._flags) {}
        Align& operator= (const Align& o) {
            _flags = o._flags;
            return *this;
        }

        /** Returns the flags of this Align */
        uint8_t flags() const noexcept { return _flags; }

        bool operator== (const Align& o) const noexcept { return _flags == o._flags; }
        bool operator== (uint8_t o) const noexcept { return _flags == o; }
        bool operator== (int o) const noexcept { return static_cast<int> (_flags) == o; }

        bool operator!= (const Align& o) const noexcept { return _flags != o._flags; }
        bool operator!= (uint8_t o) const noexcept { return _flags != o; }
        bool operator!= (int o) const noexcept { return static_cast<int> (_flags) != o; }

    private:
        uint8_t _flags = 0;
    };
