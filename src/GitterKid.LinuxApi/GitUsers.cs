using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace GitterKid.LinuxApi
{
    public class GitUsers : IEnumerable<GitUser>
    {

        IEnumerator<GitUser> IEnumerable<GitUser>.GetEnumerator() => this.CreateEnumerator();

        IEnumerator IEnumerable.GetEnumerator() => this.CreateEnumerator();
        public GitUsers() { }

        private GitUserEnumerator CreateEnumerator() => new GitUserEnumerator();

        public class GitUserEnumerator : IEnumerator<GitUser>
        {
            private IntPtr _passwdHandle;
            GitUser IEnumerator<GitUser>.Current => this.GetCurrent();

            object IEnumerator.Current => this.GetCurrent();

            public GitUserEnumerator()
            {
                this._passwdHandle = NativeMethod.GetPasswdHandle();
            }

            void IDisposable.Dispose()
            {
                NativeMethod.DisposePasswdHandle(this._passwdHandle);
            }

            bool IEnumerator.MoveNext()
            {
                int result = NativeMethod.MoveNext(this._passwdHandle);

                if (result == -2)
                {
                    NativeMethod.ResetPasswdCursor(this._passwdHandle);

                    result = NativeMethod.MoveNext(this._passwdHandle);
                }

                return result == 0;
            }

            void IEnumerator.Reset()
            {
                NativeMethod.ResetPasswdCursor(this._passwdHandle);
            }

            private GitUser GetCurrent() => new GitUser
            {
                UserId = NativeMethod.GetCurrentUserId(this._passwdHandle),
                GroupId = NativeMethod.GetCurrentGroupId(this._passwdHandle),
                UserName = Marshal.PtrToStringAnsi(NativeMethod.GetCurrentUserName(this._passwdHandle)),
                HomeDirectory = Marshal.PtrToStringAnsi(NativeMethod.GetCurrentHomeDirectory(this._passwdHandle)),
                Shell = Marshal.PtrToStringAnsi(NativeMethod.GetCurrentShell(this._passwdHandle))
            };
        }

        internal static class NativeMethod
        {
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "user_get_users")]
            internal extern static IntPtr GetPasswdHandle();
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "user_dispose_users")]
            internal extern static void DisposePasswdHandle(IntPtr passwdHandle);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "user_users_initialize_cursor")]
            internal extern static int ResetPasswdCursor(IntPtr passwdHandle);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "user_users_get_cursor")]
            internal extern static IntPtr GetCurrentCursor(IntPtr passwdHandle);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_cursor_line")]
            internal extern static IntPtr GetCurrentLine(IntPtr passwdHandle);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "user_users_cursor_move_next")]
            internal extern static int MoveNext(IntPtr passwdHandle);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_cursor_userid")]
            internal extern static int GetCurrentUserId(IntPtr passwdHandle);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_cursor_groupid")]
            internal extern static int GetCurrentGroupId(IntPtr passwdHandle);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_cursor_username")]
            internal extern static IntPtr GetCurrentUserName(IntPtr passwdHandle);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_cursor_dir")]
            internal extern static IntPtr GetCurrentHomeDirectory(IntPtr passwdHandle);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_cursor_shell")]
            internal extern static IntPtr GetCurrentShell(IntPtr passwdHandle);
        }
    }
}