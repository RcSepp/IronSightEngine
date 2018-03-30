//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.5
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


public class D3DMaterialVector : global::System.IDisposable, global::System.Collections.IEnumerable
    , global::System.Collections.Generic.IEnumerable<D3DMaterial>
 {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal D3DMaterialVector(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(D3DMaterialVector obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~D3DMaterialVector() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          IronSightEnginePINVOKE.delete_D3DMaterialVector(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public D3DMaterialVector(global::System.Collections.ICollection c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (D3DMaterial element in c) {
      this.Add(element);
    }
  }

  public bool IsFixedSize {
    get {
      return false;
    }
  }

  public bool IsReadOnly {
    get {
      return false;
    }
  }

  public D3DMaterial this[int index]  {
    get {
      return getitem(index);
    }
    set {
      setitem(index, value);
    }
  }

  public int Capacity {
    get {
      return (int)capacity();
    }
    set {
      if (value < size())
        throw new global::System.ArgumentOutOfRangeException("Capacity");
      reserve((uint)value);
    }
  }

  public int Count {
    get {
      return (int)size();
    }
  }

  public bool IsSynchronized {
    get {
      return false;
    }
  }

  public void CopyTo(D3DMaterial[] array)
  {
    CopyTo(0, array, 0, this.Count);
  }

  public void CopyTo(D3DMaterial[] array, int arrayIndex)
  {
    CopyTo(0, array, arrayIndex, this.Count);
  }

  public void CopyTo(int index, D3DMaterial[] array, int arrayIndex, int count)
  {
    if (array == null)
      throw new global::System.ArgumentNullException("array");
    if (index < 0)
      throw new global::System.ArgumentOutOfRangeException("index", "Value is less than zero");
    if (arrayIndex < 0)
      throw new global::System.ArgumentOutOfRangeException("arrayIndex", "Value is less than zero");
    if (count < 0)
      throw new global::System.ArgumentOutOfRangeException("count", "Value is less than zero");
    if (array.Rank > 1)
      throw new global::System.ArgumentException("Multi dimensional array.", "array");
    if (index+count > this.Count || arrayIndex+count > array.Length)
      throw new global::System.ArgumentException("Number of elements to copy is too large.");
    for (int i=0; i<count; i++)
      array.SetValue(getitemcopy(index+i), arrayIndex+i);
  }

  global::System.Collections.Generic.IEnumerator<D3DMaterial> global::System.Collections.Generic.IEnumerable<D3DMaterial>.GetEnumerator() {
    return new D3DMaterialVectorEnumerator(this);
  }

  global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator() {
    return new D3DMaterialVectorEnumerator(this);
  }

  public D3DMaterialVectorEnumerator GetEnumerator() {
    return new D3DMaterialVectorEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class D3DMaterialVectorEnumerator : global::System.Collections.IEnumerator
    , global::System.Collections.Generic.IEnumerator<D3DMaterial>
  {
    private D3DMaterialVector collectionRef;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public D3DMaterialVectorEnumerator(D3DMaterialVector collection) {
      collectionRef = collection;
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public D3DMaterial Current {
      get {
        if (currentIndex == -1)
          throw new global::System.InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new global::System.InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new global::System.InvalidOperationException("Collection modified.");
        return (D3DMaterial)currentObject;
      }
    }

    // Type-unsafe IEnumerator.Current
    object global::System.Collections.IEnumerator.Current {
      get {
        return Current;
      }
    }

    public bool MoveNext() {
      int size = collectionRef.Count;
      bool moveOkay = (currentIndex+1 < size) && (size == currentSize);
      if (moveOkay) {
        currentIndex++;
        currentObject = collectionRef[currentIndex];
      } else {
        currentObject = null;
      }
      return moveOkay;
    }

    public void Reset() {
      currentIndex = -1;
      currentObject = null;
      if (collectionRef.Count != currentSize) {
        throw new global::System.InvalidOperationException("Collection modified.");
      }
    }

    public void Dispose() {
        currentIndex = -1;
        currentObject = null;
    }
  }

  public void Clear() {
    IronSightEnginePINVOKE.D3DMaterialVector_Clear(swigCPtr);
  }

  public void Add(D3DMaterial x) {
    IronSightEnginePINVOKE.D3DMaterialVector_Add(swigCPtr, D3DMaterial.getCPtr(x));
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  private uint size() {
    uint ret = IronSightEnginePINVOKE.D3DMaterialVector_size(swigCPtr);
    return ret;
  }

  private uint capacity() {
    uint ret = IronSightEnginePINVOKE.D3DMaterialVector_capacity(swigCPtr);
    return ret;
  }

  private void reserve(uint n) {
    IronSightEnginePINVOKE.D3DMaterialVector_reserve(swigCPtr, n);
  }

  public D3DMaterialVector() : this(IronSightEnginePINVOKE.new_D3DMaterialVector__SWIG_0(), true) {
  }

  public D3DMaterialVector(D3DMaterialVector other) : this(IronSightEnginePINVOKE.new_D3DMaterialVector__SWIG_1(D3DMaterialVector.getCPtr(other)), true) {
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public D3DMaterialVector(int capacity) : this(IronSightEnginePINVOKE.new_D3DMaterialVector__SWIG_2(capacity), true) {
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  private D3DMaterial getitemcopy(int index) {
    D3DMaterial ret = new D3DMaterial(IronSightEnginePINVOKE.D3DMaterialVector_getitemcopy(swigCPtr, index), true);
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private D3DMaterial getitem(int index) {
    D3DMaterial ret = new D3DMaterial(IronSightEnginePINVOKE.D3DMaterialVector_getitem(swigCPtr, index), false);
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private void setitem(int index, D3DMaterial val) {
    IronSightEnginePINVOKE.D3DMaterialVector_setitem(swigCPtr, index, D3DMaterial.getCPtr(val));
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void AddRange(D3DMaterialVector values) {
    IronSightEnginePINVOKE.D3DMaterialVector_AddRange(swigCPtr, D3DMaterialVector.getCPtr(values));
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public D3DMaterialVector GetRange(int index, int count) {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3DMaterialVector_GetRange(swigCPtr, index, count);
    D3DMaterialVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new D3DMaterialVector(cPtr, true);
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Insert(int index, D3DMaterial x) {
    IronSightEnginePINVOKE.D3DMaterialVector_Insert(swigCPtr, index, D3DMaterial.getCPtr(x));
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void InsertRange(int index, D3DMaterialVector values) {
    IronSightEnginePINVOKE.D3DMaterialVector_InsertRange(swigCPtr, index, D3DMaterialVector.getCPtr(values));
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveAt(int index) {
    IronSightEnginePINVOKE.D3DMaterialVector_RemoveAt(swigCPtr, index);
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveRange(int index, int count) {
    IronSightEnginePINVOKE.D3DMaterialVector_RemoveRange(swigCPtr, index, count);
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public static D3DMaterialVector Repeat(D3DMaterial value, int count) {
    global::System.IntPtr cPtr = IronSightEnginePINVOKE.D3DMaterialVector_Repeat(D3DMaterial.getCPtr(value), count);
    D3DMaterialVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new D3DMaterialVector(cPtr, true);
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Reverse() {
    IronSightEnginePINVOKE.D3DMaterialVector_Reverse__SWIG_0(swigCPtr);
  }

  public void Reverse(int index, int count) {
    IronSightEnginePINVOKE.D3DMaterialVector_Reverse__SWIG_1(swigCPtr, index, count);
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRange(int index, D3DMaterialVector values) {
    IronSightEnginePINVOKE.D3DMaterialVector_SetRange(swigCPtr, index, D3DMaterialVector.getCPtr(values));
    if (IronSightEnginePINVOKE.SWIGPendingException.Pending) throw IronSightEnginePINVOKE.SWIGPendingException.Retrieve();
  }

}