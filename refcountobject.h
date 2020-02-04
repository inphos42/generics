#ifndef GENERICS_REFCOUNTOBJECT_H
#define GENERICS_REFCOUNTOBJECT_H

#include <cstddef>
#include <cstdint>
#include <cassert>

namespace generics {
	class RefCountObject {
	public:
		RefCountObject() : m_rc(0) {}
		virtual ~RefCountObject() {}

		inline void rcInc() { m_rc++; }
		inline void rcDec() { assert(m_rc); m_rc--; if (m_rc < 1) delete this; }

		inline int rc() const { return m_rc; }

	private:
		RefCountObject(const RefCountObject & other);
		RefCountObject & operator=(const RefCountObject & other);

		uint32_t m_rc;
	};

	template< class RCClass >
	class RCWrapper {
	public:
		RCWrapper() : m_Private(NULL) {};
		RCWrapper(RCClass * data) : m_Private(data) { if (m_Private) m_Private->rcInc(); }
		RCWrapper(const RCWrapper & other) : m_Private(other.m_Private) { if (m_Private) m_Private->rcInc(); }
		virtual ~RCWrapper() { if (m_Private) m_Private->rcDec(); }

		RCWrapper & operator=(const RCWrapper & other) {
			if (m_Private == other.m_Private)
				return *this;

			if (m_Private) m_Private->rcDec();
			m_Private = other.m_Private;
			if (m_Private) m_Private->rcInc();

			return *this;
		}

		bool operator==(const RCWrapper & other) { return m_Private == other.m_Private; }
		bool operator!=(const RCWrapper & other) { return m_Private != other.m_Private; }

		inline bool isNull() const { return !m_Private; }
	protected:
		void reset(RCClass * data) {
			if (data) {
				data->rcInc();
			}
			if (m_Private) {
				m_Private->rcDec();
			}
			m_Private = data;
		}
		
		inline RCClass * priv() const { return m_Private; }

	protected:
		RCClass * m_Private;
	};
	
	template< class RCClass >
	class RCPtr: RCWrapper<RCClass> {
		void safe_bool_func() {}
		typedef void (RCPtr<RCClass>:: * safe_bool_type) ();
	public:
		RCPtr() : RCWrapper<RCClass>() {};
		RCPtr(RCPtr const &) = default;
		explicit RCPtr(RCClass * data) : RCWrapper<RCClass>(data) {}
		RCPtr(const RCWrapper<RCClass> & other) : RCWrapper<RCClass>(other) {}
		virtual ~RCPtr() {}

		RCPtr & operator=(const RCPtr& other) {
			RCWrapper<RCClass>::operator=(other);
			return *this;
		}
		
		inline bool operator==(const RCPtr<RCClass> & other) { return RCWrapper<RCClass>::operator==(other); }
		inline bool operator!=(const RCPtr<RCClass> & other) { return RCWrapper<RCClass>::operator!=(other); }
		
		RCClass & operator*() { return *RCWrapper<RCClass>::priv();}
		const RCClass & operator*() const { return *RCWrapper<RCClass>::priv();}

		RCClass * operator->() { return RCWrapper<RCClass>::priv();}
		const RCClass * operator->() const { return RCWrapper<RCClass>::priv();}

		RCClass * priv() { return RCWrapper<RCClass>::priv(); }
		const RCClass * priv() const { return RCWrapper<RCClass>::priv(); }
		
		RCClass * get() { return priv(); }
		const RCClass * get() const { return priv(); }
		
		operator safe_bool_type() const {
			return priv() ? &RCPtr<RCClass>::safe_bool_func : 0;
		}
		void reset(RCClass * data) {
			RCWrapper<RCClass>::reset(data);
		}
	};
}

#endif
