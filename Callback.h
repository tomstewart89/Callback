#ifndef CALLBACK_H
#define CALLBACK_H

// The Slot base class, its template parameter indicates the datatype of the parameters it expects to receive. Slots can only
// be connected to Signals with identical ParameterTypes.
template <class ParameterType> class Slot
{
public:
    Slot() { }
    virtual ~Slot() { }
    // Allows the slot to be called by the signal during emmission
    virtual void operator() (ParameterType param) = 0;

    // Allows the signal to take a copy of the slot so that it can maintain an internal reference to it upon connection.
    // Essentially a virtual copy consructor.
    virtual Slot<ParameterType>* Clone() = 0;
};

// The Signal class, we can implant these into ends and allow means to connect their members to them should they want to
// receive callbacks from their children means. Ofcourse it's possible that these callbacks are made within the context of
// an interrupt so the receipient will want to be fairly quick about how they process it.
template <class ParameterType, int slots = 8> class Signal
{
    Slot<ParameterType>* connections[slots];
	int nextSlot;

public:
    Signal() { nextSlot = 0; }

    // Since the signal takes copies of all the input slots via Clone() it needs to clean up after itself when being destroyed.
    virtual ~Signal()
    {
        for(int i = 0; i < nextSlot; i++)
            delete connections[i];
    }

    // Accepts a slot of the appropriate type and adds it to its list of connections
    void Connect(Slot<ParameterType>& slot)
    {
		// If we've run out of slots
		if(nextSlot == (slots - 1))
			return;

		// Otherwise connect it up and away we go
		connections[nextSlot++] = slot.Clone(); 
	}

    // Visits each of its listeners and executes them via operator()
    void Emit(ParameterType param)
    {
        for(int i = 0; i < nextSlot; i++)
            (*connections[i])(param);
    }
};

// PointerFunctionSlot is a subclass of Slot for use with function pointers. In truth there's not really any need to wrap up
// free standing function pointers into slots since any function in C/C++ is happy to accept a raw function pointer and execute
// it. However this system allows free standing functions to be used alongside member functions or even arbitrary functor objects.
template <class ParameterType> class FunctionSlot : public Slot<ParameterType>
{
    typedef void (*FunctPtr)(ParameterType);

    // A free standing function pointer
    FunctPtr funct;

public:
    FunctionSlot(FunctPtr _funct) : funct(_funct) { }

    // Copy the slot
    Slot<ParameterType> *Clone()
            { return new FunctionSlot<ParameterType>(this->funct); }

    // Execute the slot
    void operator() (ParameterType param)
    { return (funct)(param); }
};

// MemberFunctionSlot is a subclass of Slot that allows member function pointers to be used as slots. While free standing
// pointers to functions are relatively intuitive here, Members functions need an additional template parameter, the
// owner object type and they are executed via the ->* operator.
template <class ObjectType, class ParameterType> class MethodSlot : public Slot<ParameterType>
{
    typedef void (ObjectType::*FunctPtr)(ParameterType);

    // The function pointer's owner object
    ObjectType *obj;

    // A function-pointer-to-method of class ObjectType
    FunctPtr funct;

public:
    MethodSlot(ObjectType *_obj, FunctPtr _funct) : obj(_obj), funct(_funct) { }

    // Copy the slot
    Slot<ParameterType> *Clone()
            { return new MethodSlot<ObjectType,ParameterType>(this->obj, this->funct); }

    // Execute the slot
    void operator() (ParameterType param)
    { return (obj->*funct)(param); }
};


#endif // CALLBACK_H
