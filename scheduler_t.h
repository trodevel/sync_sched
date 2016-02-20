/*

SchedulerT.

Copyright (C) 2016 Sergey Kolevatov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/


// $Revision: 3435 $ $Date:: 2016-02-19 #$ $Author: serge $

#ifndef SYNC_SCHED_SCHEDULER_T_H
#define SYNC_SCHED_SCHEDULER_T_H

#include <map>                  // std::map
#include <vector>               // std::vector

namespace sync_sched
{

template <class _BASE, class _EVENT, class _TIMEVAR = uint32_t>
class SchedulerT
{
public:

private:
    typedef std::vector<_EVENT>             EventsList;

    typedef std::map<_TIMEVAR, EventsList>  MapTimeToEvents;

    typedef typename MapTimeToEvents::value_type    PairTimeEvent;

public:

    SchedulerT( _BASE & base ):
        base_( base ),
        has_next_time_( false ),
        has_last_time_( false )
    {
    }

    void iterate( _TIMEVAR time )
    {
        if( has_last_time_ )
        {
            if( time <= last_time_ )
                throw std::logic_error( "iterate: time iterated twice" );
        }
        else
        {
            has_last_time_  = true;
        }

        last_time_ = time;

        if( has_next_time_ == false || time < next_time_ )
            return;

        if( time >= next_time_ )
        {
            if( events_.empty() )
            {
                throw std::logic_error( "event list is empty" );
            }

            auto it = events_.begin();

            if( it->first !=  next_time_ )
            {
                throw std::logic_error( "time of first event doesn't match next_time_" );
            }

            EventsList & l = it->second;

            for( auto & ev : l )
            {
                base_.on_event( time, ev );
            }

            events_.erase( it );

            if( events_.empty() == false )
            {
                next_time_ = events_.begin()->first;
            }
            else
            {
                has_next_time_  = false;
            }
        }
    }

    void add_event( _TIMEVAR time, _EVENT event )
    {
        auto it = events_.find( time );

        if( it == events_.end() )
        {
            EventsList l;

            l.push_back( event );

            events_.insert( PairTimeEvent( time, l ) );

            if( has_next_time_ )
            {
                if( time < next_time_ )
                {
                    next_time_      = time;
                }
            }
            else
            {
                has_next_time_  = true;
                next_time_      = time;
            }
        }
        else
        {
            EventsList & l = it->second;

            l.push_back( event );
        }
    }

private:

    _BASE           & base_;
    bool            has_next_time_;
    bool            has_last_time_;
    _TIMEVAR        next_time_;
    _TIMEVAR        last_time_;
    MapTimeToEvents events_;
};


} // namespace sync_sched

#endif  // SYNC_SCHED_SCHEDULER_T_H
