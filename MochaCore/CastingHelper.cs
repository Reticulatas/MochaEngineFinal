using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Reflection;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MochaScriptSystem 
{
    internal class CastingHelper
    {
        private static Dictionary<Type, Func<IntPtr, bool, object>> constructorCache =
            new Dictionary<Type, Func<IntPtr, bool, object>>();

        private static Func<IntPtr, bool, object> CreateConstructorDelegate<T>()
        {
            // We need to first grab the reflection information for the more derived type we're casting to.
            // SWIG has protected constructors we'll be able to call that take 2 parameters, the native pointer
            // to the type and whether or not SWIG owns the memory
            ConstructorInfo ctor = typeof(T).GetConstructor(
                BindingFlags.Instance | BindingFlags.NonPublic, null,
                CallingConventions.HasThis, new Type[] { typeof(IntPtr), typeof(bool) }, null);

            // Lets emit some IL that will allow us to construct our type much
            // faster than using reflection.
            DynamicMethod dm = new DynamicMethod("Create", typeof(object),
                new Type[] { typeof(IntPtr), typeof(bool) }, typeof(T), true);
            ILGenerator il = dm.GetILGenerator();
            il.Emit(OpCodes.Ldarg_0);
            il.Emit(OpCodes.Ldarg_1);
            il.Emit(OpCodes.Newobj, ctor);
            il.Emit(OpCodes.Ret);

            // Create the delegate for the dynamic method, which will further improve the calling speed.
            return (Func<IntPtr, bool, object>)dm.CreateDelegate(typeof(Func<IntPtr, bool, object>));
        }

        public static T CastTo<T>(MochaInterface.IMeta self) where T : MochaInterface.IMeta
        {
            if (self == null)
                return null;

            // Check if the object is actually the type we're trying to cast to.
            if (self.IsKindOf<T>())
            {
                Type type = typeof(T);

                // Check if we've already cached the factory function.
                Func<IntPtr, bool, object> factory;
                if (!constructorCache.TryGetValue(type, out factory))
                {
                    factory = CreateConstructorDelegate<T>();
                    constructorCache.Add(type, factory);
                }

                // Call the factory function and set the casted source to the
                // current object.
                T castedObject = (T)factory(MochaInterface.IMeta.get_static_CPtr(self).Handle, false);
                (castedObject as MochaInterface.IMeta).m_castedSource = self;

                return castedObject;
            }
            return null;
        }

        public static object castTo(object fromObj, Type toType)
        {
            object retval = null;

            MochaInterface.IMeta fromObj2 = fromObj as MochaInterface.IMeta;
            HandleRef hr = fromObj2.get_CPtr();
            BindingFlags flags = BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Public;
            CultureInfo culture = null;
            IntPtr cPtr = hr.Handle;
            object[] parameters = { cPtr, false };
            object toObj = Activator.CreateInstance(toType, flags, null, parameters, culture);

            // make sure it actually is what we think it is
            if (fromObj.GetType().IsInstanceOfType(toObj))
            {
                return toObj;
            }

            return retval;
        }
    }
}
