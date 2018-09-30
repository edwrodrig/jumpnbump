//
// Created by edwin on 30-09-18.
//

#include <algorithm>
#include "map_elements_t.h"


map_elements_t map_elements_t::collide(const bounding_box_t& bounding_box) const {
    map_elements_t elements;
    for ( auto& element : this->elements ) {
        if ( element.bounding_box.collide(bounding_box) )
            elements.elements.emplace_back(element);
    }
    return elements;
}

bool map_elements_t::is_wall() const {
    return
        std::find_if(
                this->elements.cbegin(),
                this->elements.cend(),
                [](const map_element_t& a) { return a.is_wall(); }
                ) != std::end(this->elements);
}

bool map_elements_t::is_ceil() const {
    return
        std::find_if(
                this->elements.cbegin(),
                this->elements.cend(),
                [](const map_element_t& a) { return a.is_ceil(); }
        ) != std::end(this->elements);
}

const map_element_t& map_elements_t::get_rightmost_wall() const {
    return *std::max_element(
            this->elements.cbegin(),
            this->elements.cend(),
            [](const map_element_t& a, const map_element_t& b) { //a less than b
                if ( !a.is_wall() &&  b.is_wall() ) return true;
                if (  a.is_wall() && !b.is_wall() ) return false;

                //more is more right
                return a.bounding_box.get_right() < b.bounding_box.get_right();
            }
    );
}

const map_element_t& map_elements_t::get_lowest_ceil() const {
    return *std::max_element(
            this->elements.cbegin(),
            this->elements.cend(),
            [](const map_element_t& a, const map_element_t& b) { //a less than b
                if ( !a.is_ceil() &&  b.is_ceil() ) return true;
                if (  a.is_ceil() && !b.is_ceil() ) return false;

                //more is lower
                return a.bounding_box.get_bottom() < b.bounding_box.get_bottom();
            }
    );
}

const map_element_t& map_elements_t::get_leftmost_wall() const {
    return *std::max_element(
            this->elements.cbegin(),
            this->elements.cend(),
            [](const map_element_t& a, const map_element_t& b) { //a less than b
                if ( !a.is_wall() &&  b.is_wall() ) return true;
                if (  a.is_wall() && !b.is_wall() ) return false;

                //less if more left
                return a.bounding_box.get_left() > b.bounding_box.get_left();
            }
    );

}