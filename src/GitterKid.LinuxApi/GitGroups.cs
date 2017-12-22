using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Collections;

namespace GitterKid.LinuxApi
{
    public class GitGroups : IEnumerable<GitGroup>
    {

        IEnumerator<GitGroup> IEnumerable<GitGroup>.GetEnumerator()
        {
            throw new NotImplementedException();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            throw new NotImplementedException();
        }

        public GitGroups() { }

        private GitGroupEnumerator CreateEnumerator() => new GitGroupEnumerator();

        public class GitGroupEnumerator : IEnumerator<GitGroup>
        {
            private IntPtr _groupHandle;
            GitGroup IEnumerator<GitGroup>.Current => this.GetCurrent();

            object IEnumerator.Current => this.GetCurrent();

            public GitGroupEnumerator()
            {
                this._groupHandle = NativeMethod.BuildGroupHandle();
                NativeMethod.OpenGroup(this._groupHandle);
            }

            void IDisposable.Dispose()
            {
                NativeMethod.DisposeGroup(this._groupHandle);
            }

            bool IEnumerator.MoveNext()
            {
                int result = NativeMethod.MoveNextCursor(this._groupHandle);

                if (result == -3)
                {
                    NativeMethod.ResetCursor(this._groupHandle);

                    result = NativeMethod.MoveNextCursor(this._groupHandle);
                }

                return result == 0;
            }

            void IEnumerator.Reset()
            {
                NativeMethod.ResetCursor(this._groupHandle);
            }

            private GitGroup GetCurrent() => new GitGroup(NativeMethod.GetCurrentGroup(this._groupHandle));
        }

        internal static class NativeMethod
        {
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "build_group_handle")]
            internal extern static IntPtr BuildGroupHandle();
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "open_group")]
            internal extern static int OpenGroup(IntPtr groupHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "dispose_group")]
            internal extern static void DisposeGroup(IntPtr groupHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "reset_group_cursor")]
            internal extern static int ResetCursor(IntPtr groupHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_current_group")]
            internal extern static IntPtr GetCurrentGroup(IntPtr groupHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "move_group_cursor_next")]
            internal extern static int MoveNextCursor(IntPtr groupHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_group_name")]
            internal extern static IntPtr GetGroupName(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_group_passwd")]
            internal extern static IntPtr GetGroupPassword(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_group_gid")]
            internal extern static int GetGroupId(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_group_member_cursor")]
            internal extern static IntPtr GetGroupMemberCursor(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_memeber_cursor_size")]
            internal extern static int GetMemberCursorSize();
        }
    }
}