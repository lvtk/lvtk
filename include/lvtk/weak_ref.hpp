// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstddef>
#include <memory>
#include <utility>

namespace lvtk {

/** Status of a weak-referenced object.
    Used by WeakRef to monitor deleted pointers.
    @see WeakRef
 */
template <class T>
class WeakStatus {
public:
    using owner_type = T;

    WeakStatus() : status (std::make_shared<Status>()) {}
    WeakStatus (const WeakStatus& o) { status = o.status; }
    WeakStatus (WeakStatus&& o) { status = std::move (o.status); }
    ~WeakStatus() { status.reset(); }

    WeakStatus& operator= (const WeakStatus& o) {
        status = o.status;
        return *this;
    }
    WeakStatus& operator= (WeakStatus&& o) {
        status = std::move (o.status);
        return *this;
    }

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

    /** Returns true if the pointer has not been deleted. */
    bool valid() const noexcept {
        return status != nullptr && status->ptr != nullptr;
    }

    /** Initialize or clear the pointer.
        Call without passing a ptr to indicate deleted.

        @param ptr  Set to `this` in your class ctor.
    */
    void reset (T* ptr = nullptr) {
        if (status != nullptr)
            status->ptr = ptr;
    }

    T* get() const noexcept { return status != nullptr ? status->ptr : nullptr; }

private:
    struct Status {
        owner_type* ptr { nullptr };
    };
    std::shared_ptr<Status> status;
};

/** A weak refrenceable object ptr.

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
template <class Obj>
class WeakRef {
public:
    WeakRef() = default;
    WeakRef (Obj* obj)
        : _status (obj != nullptr ? Obj::lvtk_weak_status (obj) : WeakStatus<Obj>()) {}

    /** Returns the reference if not deleted, otherwise nullptr */
    Obj* lock() const noexcept {
        return _status != nullptr ? _status.get() : nullptr;
    }

    /** Returns the ptr without checking if deleted or not.
        Use with caution
    */
    Obj* get() const noexcept {
        return _status.get();
    }

    /** Cast the internal pointer to specified type. */
    template <class T>
    T* as() const noexcept {
        return dynamic_cast<T*> (lock());
    }

    /** Returns true if the ptr hasn't been deleted */
    bool valid() const noexcept { return nullptr != lock(); }

    Obj* operator->() const noexcept { return _status.get(); }
    Obj& operator*() noexcept { return *_status.get(); }
    const Obj& operator*() const noexcept { return *_status.get(); }

    bool operator== (Obj* o) const noexcept { return lock() == o; }
    bool operator== (const Obj* o) const noexcept { return lock() == o; }
    bool operator== (const WeakRef& o) const noexcept { return lock() == o.lock(); }
    bool operator== (std::nullptr_t n) const noexcept { return lock() == n; }

    bool operator!= (Obj* o) const noexcept { return lock() != o; }
    bool operator!= (const Obj* o) const noexcept { return lock() != o; }
    bool operator!= (const WeakRef& o) const noexcept { return lock() != o.lock(); }
    bool operator!= (std::nullptr_t n) const noexcept { return lock() != n; }
    operator bool() const noexcept { return lock() != nullptr; }

private:
    WeakStatus<Obj> _status;
};

} // namespace lvtk

#define LVTK_WEAK_REFABLE_WITH_MEMBER(klass, member)               \
    friend class lvtk::WeakRef<klass>;                             \
    lvtk::WeakStatus<klass> member;                                \
    static lvtk::WeakStatus<klass> lvtk_weak_status (klass* obj) { \
        return obj->member;                                        \
    }

/**
    A WeakStatus/WeakRef implementation macro.
    
    Use it to add boiler plate code required to use WeakRef.
    NOTE: this adds a _weak_status member in your class. If you need a custom
    member name, use LVTK_WEAK_REFABLE_WITH_MEMBER instead.

    @param klass The class name to use.

    @code
    class MyObject
    {
    public:
        MyObject() {
            _weak_status.reset (this);
        }
        ~MyObject() {
            _weak_status.reset();
        }

    private:
        LVTK_WEAK_REFABLE (MyObject)
    };
    @endcode

    @see WeakRef, WeakStatus
*/
#define LVTK_WEAK_REFABLE(klass) LVTK_WEAK_REFABLE_WITH_MEMBER (klass, _weak_status)
