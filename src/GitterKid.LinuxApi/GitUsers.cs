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
                this._passwdHandle = NativeMethod.BuildPasswdHandle();
                NativeMethod.OpenPasswd(this._passwdHandle);
            }

            void IDisposable.Dispose()
            {
                NativeMethod.DisposePasswd(this._passwdHandle);
            }

            bool IEnumerator.MoveNext()
            {
                int result = NativeMethod.MoveNextCursor(this._passwdHandle);

                if (result == -3)
                {
                    NativeMethod.ResetCursor(this._passwdHandle);

                    result = NativeMethod.MoveNextCursor(this._passwdHandle);
                }

                return result == 0;
            }

            void IEnumerator.Reset()
            {
                NativeMethod.ResetCursor(this._passwdHandle);
            }

            private GitUser GetCurrent() => new GitUser(NativeMethod.GetCurrentPasswd(this._passwdHandle));
        }

        internal static class NativeMethod
        {
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "build_passwd_handle")]
            internal extern static IntPtr BuildPasswdHandle();
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "open_passwd")]
            internal extern static int OpenPasswd(IntPtr passwdHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "dispose_passwd")]
            internal extern static int DisposePasswd(IntPtr passwdHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "reset_passwd_cursor")]
            internal extern static int ResetCursor(IntPtr passwdHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_current_passwd")]
            internal extern static IntPtr GetCurrentPasswd(IntPtr passwdHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "move_passwd_cursor_next")]
            internal extern static int MoveNextCursor(IntPtr passwdHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_passwd_username")]
            internal extern static IntPtr GetPasswdUserName(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_passwd_dir")]
            internal extern static IntPtr GetPasswdHomeDirectory(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_passwd_gecos")]
            internal extern static IntPtr GetPasswdGecos(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_passwd_uid")]
            internal extern static int GetPasswdUserId(IntPtr entryHandle);

            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_passwd_gid")]
            internal extern static int GetPasswdGroupId(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_passwd_passwd")]
            internal extern static IntPtr GetPasswdPassword(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_passwd_shell")]
            internal extern static IntPtr GetPasswdShell(IntPtr entryHandle);
        }
    }
}