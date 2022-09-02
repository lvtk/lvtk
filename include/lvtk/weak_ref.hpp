/* 
    Copyright (c) 2022, Michael Fisher <mfisher@kushview.net>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#pragma once

#include <memory>

namespace lvtk {

/** Status of a weak-referenced object.
    @see lvtk::WeakRef
 */
template<class T>
class WeakStatus {
public:
    using owner_type = T;

    WeakStatus() : status (std::make_shared<Status>()) {}
    WeakStatus (const WeakStatus& o) { status = o.status; }
    WeakStatus (WeakStatus&& o) { status = std::move (o.status); }
    ~WeakStatus() { status.reset(); }

    WeakStatus& operator= (const WeakStatus& o) {
        status = o.status; return *this; }
    WeakStatus& operator= (WeakStatus&& o) { 
        status = std::move (o.status); return *this; }
    
    bool operator== (const T* o) const noexcept {
        return status != nullptr && status->ptr == o;
    }
    bool operator== (std::nullptr_t o) const noexcept {
        return status != nullptr && status->ptr == o;
    }

    bool operator!= (const T* o) const noexcept {
        return status != nullptr && status->ptr != o;
    }
    bool operator!= (std::nullptr_t o) const noexcept {
        return status != nullptr && status->ptr != o;
    }

    bool valid() const noexcept { 
        return status != nullptr && status->ptr != nullptr;
    }

    void reset (T* ptr = nullptr) {
        if (status != nullptr)
            status->ptr = ptr;
    }

    T* get() const noexcept { return status != nullptr ? status->ptr : nullptr; }

private:
    struct Status { owner_type* ptr = nullptr; };
    std::shared_ptr<Status> status;
};

/** A weak refereable object ptr.

    To use it:
    1) Add a WeakStatus<MyObject> to your class.
    2) Implement the accessor static method:
        static WeakStatus<MyObject> weak_status (MyObject* ptr);
    3) In the ctor, initlize the status.
    4) In the dtor, clear the status.

    See the @see LVTK_WEAK_REFABLE macro to easily implement boiler plate
    details.

    @code
    class MyObject
    {
    public:
        MyObject() {
            weak_status.reset (this);
        }
        ~MyObject() {
            weak_status.reset();
        }

    private:
        friend c
        WeakStatus<MyObject> m_weak;
        static WeakStatus<MyObject> weak_status (MyObject* obj) {
            return obj->m_weak;
        }
    };
    @endcode
*/
template<class Obj>
class WeakRef {
public:
    WeakRef() = default;
    WeakRef (Obj* obj) { _status = Obj::lvtk_weak_status (obj); }
    
    /** Returns the reference if not deleted, otherwise nullptr */
    Obj* lock() const noexcept {
        return _status != nullptr ? _status.get() : nullptr;
    }

    /** Cast the internal pointer to specified type. */
    template<class T>
    T* as() const noexcept {
        return dynamic_cast<T*> (lock());
    }

    /** Returns true if the ptr hasn't been deleted */
    bool valid() const noexcept { return nullptr != lock(); }

private:
    WeakStatus<Obj> _status;
};

}

/**
    Macro to make a Weak Referenceable object.
    Use it to implement boiler plate code required for a WeakRef.

    @param klass    The class name to use.
    @param member   The member variable name of the WeakStatus.
    
    @code
    class MyObject
    {
    public:
        MyObject() {
            weak_status.reset (this);
        }
        ~MyObject() {
            weak_status.reset();
        }

    private:
        LVTK_WEAK_REFABLE (MyObject, weak_status)
    };
    @endcode

    @see WeakRef, WeakStatus
*/
#define LVTK_WEAK_REFABLE(klass,member) \
friend class lvtk::WeakRef<klass>; \
lvtk::WeakStatus<klass> member; \
static lvtk::WeakStatus<klass> lvtk_weak_status (klass* obj) { \
    return obj->member; \
}
