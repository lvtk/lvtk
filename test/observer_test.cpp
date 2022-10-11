// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include <boost/signals2.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE (Observer)

BOOST_AUTO_TEST_CASE (connectivity) {
    BOOST_ASSERT (true);
}

BOOST_AUTO_TEST_SUITE_END()

#if 0
// Document/View sample for Boost.Signals2.
// Expands on doc_view.cpp example by using automatic
// connection management.
//
// Copyright Keith MacDonald 2005.
// Copyright Frank Mori Hess 2009.
//
// Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// For more information, see http://www.boost.org

#    include <boost/bind.hpp>
#    include <boost/shared_ptr.hpp>
#    include <boost/signals2/signal.hpp>
#    include <iostream>
#    include <string>

#    if 0
class Document {
public:
    typedef boost::signals2::signal<void()> signal_t;

public:
    Document() {}

    /* connect a slot to the signal which will be emitted whenever
    text is appended to the document. */
    boost::signals2::connection connect (const signal_t::slot_type& subscriber) {
        return m_sig.connect (subscriber);
    }

    void append (const char* s) {
        m_text += s;
        m_sig();
    }

    const std::string& getText() const {
        return m_text;
    }

private:
    signal_t m_sig;
    std::string m_text;
};

class TextView {
public:
    // static factory function that sets up automatic connection tracking
    static boost::shared_ptr<TextView> create (Document& doc) {
        boost::shared_ptr<TextView> new_view (new TextView (doc));
        {
            typedef Document::signal_t::slot_type slot_type;
            slot_type myslot (&TextView::refresh, new_view.get());
            doc.connect (myslot.track (new_view));
        }
        return new_view;
    }

    void refresh() const {
        std::cout << "TextView: " << m_document.getText() << std::endl;
    }

private:
    // private constructor to force use of static factory function
    TextView (Document& doc) : m_document (doc) {}

    Document& m_document;
};

#    endif

template <typename T>
using Signal = boost::signals2::signal<T>;

#    if 0
class WidgetObserver {
public:
    WidgetObserver() {
        _slots = std::make_shared<Receiver>();
    }

protected:
    virtual void elevation_changed (Widget*) { }

private:
    struct Receiver {
        void _elevation_changed
    };

    std::shared_ptr<Receiver> _slots;
};
#    endif
#endif
