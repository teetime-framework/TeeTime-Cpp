/**
 * Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

namespace teetime
{
  template<typename T>
  class alignas(alignof(T)) Optional final
  {
  public:
    Optional()
     : m_ptr(nullptr)
    {
    }

    explicit Optional(const T& t)
     : m_ptr(reinterpret_cast<T*>(&m_buffer[0]))
    {
      new (&m_buffer[0]) T(t);
    }

    explicit Optional(T&& t)
     : m_ptr(reinterpret_cast<T*>(&m_buffer[0]))    
    {
      new (&m_buffer[0]) T(std::move(t));
    }   

    Optional(const Optional& rhs)
      : m_ptr(nullptr)      
    {
      if(rhs.m_ptr)
      {
        new (&m_buffer[0]) T(*rhs);        
        m_ptr = reinterpret_cast<T*>(&m_buffer[0]);
      }
    }

    Optional(Optional&& rhs)
     : m_ptr(nullptr)
    {
      if(rhs.m_ptr)
      {
        new (&m_buffer[0]) T(std::move(*rhs));
        m_ptr = reinterpret_cast<T*>(&m_buffer[0]);
      }
    }    

    ~Optional()
    {
      reset();
    }

    operator bool() const
    {
      return !!m_ptr;
    }

    void reset()
    {
      if(m_ptr)
      {
        m_ptr->~T();
        m_ptr = nullptr;
      }
    }

    T& operator*()
    {
      return *m_ptr;
    }

    const T& operator*() const
    {
      return *m_ptr;
    }  

    void set(const T& t)
    {
      reset();
      new (&m_buffer[0]) T(t);
      m_ptr = reinterpret_cast<T*>(&m_buffer[0]);
    }

    void set(T&& t)
    {
      reset();
      new (&m_buffer[0]) T(std::move(t));
      m_ptr = reinterpret_cast<T*>(&m_buffer[0]);
    }  

  private:
    char m_buffer[sizeof(T)];    
    T*   m_ptr;
  };
}