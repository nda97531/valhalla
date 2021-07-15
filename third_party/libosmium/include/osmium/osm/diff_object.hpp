#ifndef OSMIUM_OSM_DIFF_OBJECT_HPP
#define OSMIUM_OSM_DIFF_OBJECT_HPP

/*

This file is part of Osmium (https://osmcode.org/libosmium).

Copyright 2013-2021 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <osmium/osm/item_type.hpp>
#include <osmium/osm/object.hpp>
#include <osmium/osm/timestamp.hpp>
#include <osmium/osm/types.hpp>

#include <cassert>

namespace osmium {

    class Node;
    class Way;
    class Relation;

    /**
     * A DiffObject holds pointers to three OSMObjects, the current object,
     * the previous, and the next. They always have the same type (Node, Way,
     * or Relation) and the same ID, but may have different versions.
     *
     * It is used when iterating over OSM files with history data to make
     * working with versioned OSM objects easier. Because you have access to
     * the previous and next objects as well as the current one, comparisons
     * between object versions is easy.
     *
     * If the current object is the first version available, the previous
     * pointer must be the same as the current one. If the current object is
     * the last version available, the next pointer must be the same as the
     * current one.
     *
     * DiffObjects are immutable.
     */
    class DiffObject {

        const osmium::OSMObject* m_prev = nullptr;
        const osmium::OSMObject* m_curr = nullptr;
        const osmium::OSMObject* m_next = nullptr;

    public:

        /**
         * Default construct an empty DiffObject. Most methods of this class
         * can not be called on empty DiffObjects.
         */
        DiffObject() noexcept = default;

        /**
         * Construct a non-empty DiffObject from the given OSMObjects. All
         * OSMObjects must be of the same type (Node, Way, or Relation) and
         * have the same ID.
         */
        DiffObject(const osmium::OSMObject& prev, const osmium::OSMObject& curr, const osmium::OSMObject& next) noexcept :
            m_prev(&prev),
            m_curr(&curr),
            m_next(&next) {
            assert(prev.type() == curr.type() && curr.type() == next.type());
            assert(prev.id()   == curr.id()   && curr.id()   == next.id());
        }

        /**
         * Check whether the DiffObject was created empty.
         */
        bool empty() const noexcept {
            return m_prev == nullptr;
        }

        /**
         * Get the previous object stored.
         *
         * @pre DiffObject must not be empty.
         */
        const osmium::OSMObject& prev() const noexcept {
            assert(m_prev && m_curr && m_next);
            return *m_prev;
        }

        /**
         * Get the current object stored.
         *
         * @pre DiffObject must not be empty.
         */
        const osmium::OSMObject& curr() const noexcept {
            assert(m_prev && m_curr && m_next);
            return *m_curr;
        }

        /**
         * Get the next object stored.
         *
         * @pre DiffObject must not be empty.
         */
        const osmium::OSMObject& next() const noexcept {
            assert(m_prev && m_curr && m_next);
            return *m_next;
        }

        /**
         * Is the current object version the first (with this type and ID)?
         *
         * @pre DiffObject must not be empty.
         */
        bool first() const noexcept {
            assert(m_prev && m_curr && m_next);
            return m_prev == m_curr;
        }

        /**
         * Is the current object version the last (with this type and ID)?
         *
         * @pre DiffObject must not be empty.
         */
        bool last() const noexcept {
            assert(m_prev && m_curr && m_next);
            return m_curr == m_next;
        }

        /**
         * Return the type of the current object.
         *
         * @pre DiffObject must not be empty.
         */
        osmium::item_type type() const noexcept {
            assert(m_prev && m_curr && m_next);
            return m_curr->type();
        }

        /**
         * Return the ID of the current object.
         *
         * @pre DiffObject must not be empty.
         */
        osmium::object_id_type id() const noexcept {
            assert(m_prev && m_curr && m_next);
            return m_curr->id();
        }

        /**
         * Return the version of the current object.
         *
         * @pre DiffObject must not be empty.
         */
        osmium::object_version_type version() const noexcept {
            assert(m_prev && m_curr && m_next);
            return m_curr->version();
        }

        /**
         * Return the changeset ID of the current object.
         *
         * @pre DiffObject must not be empty.
         */
        osmium::changeset_id_type changeset() const noexcept {
            assert(m_prev && m_curr && m_next);
            return m_curr->changeset();
        }

        /**
         * Return the timestamp when the current object version was created.
         *
         * @pre DiffObject must not be empty.
         */
        osmium::Timestamp start_time() const noexcept {
            assert(m_prev && m_curr && m_next);
            return m_curr->timestamp();
        }

        /**
         * Return the timestamp when the current version of the object is
         * not valid any more, ie the time when the next version of the object
         * is valid. If this is the last version of the object, this will
         * return a special "end of time" timestamp that is guaranteed to
         * be larger than any normal timestamp.
         *
         * @pre DiffObject must not be empty.
         */
        osmium::Timestamp end_time() const noexcept {
            assert(m_prev && m_curr && m_next);
            return last() ? osmium::end_of_time() : m_next->timestamp();
        }

        /**
         * Current object version is valid between time "from" (inclusive) and
         * time "to" (not inclusive).
         *
         * This is a bit more complex than you'd think, because we have to
         * handle the case properly where the start_time() == end_time().
         *
         * @pre DiffObject must not be empty.
         */
        bool is_between(const osmium::Timestamp& from, const osmium::Timestamp& to) const noexcept {
            assert(m_prev && m_curr && m_next);
            return start_time() < to &&
                   ((start_time() != end_time() && end_time() >  from) ||
                    (start_time() == end_time() && end_time() >= from));
        }

        /**
         * Current object version is visible at the given timestamp.
         *
         * @pre DiffObject must not be empty.
         */
        bool is_visible_at(const osmium::Timestamp& timestamp) const noexcept {
            assert(m_prev && m_curr && m_next);
            return start_time() <= timestamp && end_time() > timestamp && m_curr->visible();
        }

    }; // class DiffObject

    template <typename T>
    class DiffObjectDerived : public DiffObject {

    public:

        DiffObjectDerived(const T& prev, const T& curr, const T& next) noexcept :
            DiffObject(prev, curr, next) {
        }

        const T& prev() const noexcept {
            return static_cast<const T&>(DiffObject::prev());
        }

        const T& curr() const noexcept {
            return static_cast<const T&>(DiffObject::curr());
        }

        const T& next() const noexcept {
            return static_cast<const T&>(DiffObject::next());
        }

    }; // class DiffObjectDerived

    using DiffNode     = DiffObjectDerived<osmium::Node>;
    using DiffWay      = DiffObjectDerived<osmium::Way>;
    using DiffRelation = DiffObjectDerived<osmium::Relation>;

} // namespace osmium

#endif // OSMIUM_OSM_DIFF_OBJECT_HPP
