using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace GitterKid.LinuxApi
{
    public class GitUsers
    {
        private IntPtr handle;

        public GitUsers()
        {
            this.Initialize();
        }

        unsafe private void Initialize()
        {
            this.handle = NativeMethod.GetPasswds();
            if (this.handle == IntPtr.Zero)
            {
                throw new Exception();
            }
        }

        public string FirstLine()
        {
            if (NativeMethod.InitializePasswdsCursor(this.handle) != 0)
            {
                throw new NullReferenceException("passwd is null");
            }

            return Marshal.PtrToStringAnsi(NativeMethod.GetCurrentLine(this.handle));
        }

        internal static class NativeMethod
        {
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "user_get_users")]
            internal extern static IntPtr GetPasswds();

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "user_users_initialize_cursor")]
            internal extern static int InitializePasswdsCursor(IntPtr passwds);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "user_users_get_cursor")]
            internal extern static IntPtr GetCurrentCursor(IntPtr passwds);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_cursor_line")]
            internal extern static IntPtr GetCurrentLine(IntPtr passwds);
        }
    }
}