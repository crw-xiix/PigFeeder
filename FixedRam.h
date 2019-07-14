
#ifndef __FixedRam_H__
#define __FixedRam_H__ 1


template <typename T>
class FixedRam {
protected:
	static int		taskSizeBytes;
	static uint8_t	*ramBuffer; //The data area 
#ifdef TESTSETUP 
	static uint8_t	*ramNameBuffer; //Just the name for debugging
#endif
	static uint8_t	*ramPtr;  //New heap location
public:
	void* allocate(size_t size) {
		uint8_t *temp = ramPtr - size;
		if (temp < ramBuffer) return NULL;
		ramPtr = temp;
		return temp;
	}
	void Reset() {
		ramPtr = (ramBuffer + taskSizeBytes);
	}
	FixedRam<T>(const char* iname, int sz) {
		FixedRam<T>::taskSizeBytes = sz;
		ramBuffer = new uint8_t[taskSizeBytes];
		Reset();
#ifdef TESTSETUP 
		//This isn't cool, but it's needed for now.  It's only once......
		FixedRam<T>::ramNameBuffer = new uint8_t[strlen(iname) + 1];
		strcpy((char *)ramNameBuffer, iname);
#endif
	}
};

/*
template<typename T>
uint8_t *FixedRam<T>::ramNameBuffer = NULL;
*/
template<typename T>
uint8_t *FixedRam<T>::ramBuffer = NULL;

template<typename T>
uint8_t *FixedRam<T>::ramPtr = NULL;

template<typename T>
int FixedRam<T>::taskSizeBytes = 1024;

#endif
