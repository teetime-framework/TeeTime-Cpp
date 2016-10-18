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
  class Optional final
  {
  public:
    Optional()
     : m_hasValue(false)
    {
    }

    explicit Optional(const T& t)
     : m_hasValue(false)
    {
      new (ptr()) T(t);
      m_hasValue = true;
    }

    explicit Optional(T&& t)
     : m_hasValue(false)
    {
      new (ptr()) T(std::move(t));
      m_hasValue = true;
    }   

    Optional(const Optional& rhs)
      : m_hasValue(false)      
    {
      if(rhs.m_hasValue)
      {
        new (ptr()) T(*rhs.ptr());
        m_hasValue = true;
      }
    }

    Optional(Optional&& rhs)
     : m_hasValue(false)
    {
      if(rhs.m_hasValue)
      {
        new (ptr()) T(std::move(*rhs.ptr()));
        m_hasValue = true;
        rhs.reset();
      }
    }    

    ~Optional()
    {
      reset();
    }

    operator bool() const
    {
      return m_hasValue;
    }

    void reset()
    {
      if(m_hasValue)
      {
        ptr()->~T();
        m_hasValue = false;
      }
    }

    T& operator*()
    {
      return *ptr();
    }

    const T& operator*() const
    {
      return *ptr();
    }  

    void set(const T& t)
    {
      reset();
      new (ptr()) T(t);
      m_hasValue = true;
    }

    void set(T&& t)
    {
      reset();
      new (ptr()) T(std::move(t));
      m_hasValue = true;
    }  

  private:
    inline T* ptr()
    {
      return reinterpret_cast<T*>(&m_buffer[0]);
    }

    inline const T* ptr() const
    {
      return reinterpret_cast<const T*>(&m_buffer[0]);
    }    

    alignas(T) char m_buffer[sizeof(T)];    
    bool m_hasValue;
  };
}