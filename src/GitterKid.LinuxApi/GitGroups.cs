using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Collections;

namespace GitterKid.LinuxApi
{
    public class GitGroups : IEnumerable<GitGroup>, IDisposable
    {
        private IntPtr _groupHandle;
        private bool disposedValue = false;

        IEnumerator<GitGroup> IEnumerable<GitGroup>.GetEnumerator() => this.CreateEnumerator();

        IEnumerator IEnumerable.GetEnumerator() => this.CreateEnumerator();

        public GitGroups()
        {
            this._groupHandle = NativeMethod.BuildGroupHandle();
            NativeMethod.OpenGroup(this._groupHandle);
        }

        private GitGroupEnumerator CreateEnumerator() => new GitGroupEnumerator(this._groupHandle);

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    NativeMethod.DisposeGroup(this._groupHandle);
                }
                disposedValue = true;
            }
        }

        void IDisposable.Dispose()
        {
            Dispose(true);
        }

        public class GitGroupEnumerator : IEnumerator<GitGroup>
        {
            private IntPtr _groupHandle;
            private bool _isFirst;

            GitGroup IEnumerator<GitGroup>.Current => this.GetCurrent();
            object IEnumerator.Current => this.GetCurrent();

            public GitGroupEnumerator(IntPtr groupHandle)
            {
                this._groupHandle = groupHandle;
                NativeMethod.ResetCursor(this._groupHandle);
                this._isFirst = true;
            }

            void IDisposable.Dispose() { }

            bool IEnumerator.MoveNext()
            {
                if (this._isFirst)
                {
                    this._isFirst = false;
                    return NativeMethod.GetCurrentGroup(this._groupHandle) != IntPtr.Zero;
                }
                else
                {
                    return NativeMethod.MoveNextCursor(this._groupHandle) == 0;
                }
            }

            void IEnumerator.Reset()
            {
                NativeMethod.ResetCursor(this._groupHandle);
                this._isFirst = true;
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
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "reset_group_member_cursor")]
            internal extern static IntPtr ResetMemberCursor(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_current_group_member_name")]
            internal extern static IntPtr GetCurrentMemberName(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "group_member_move_next")]
            internal extern static int GroupMemberMoveNext(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "dispose_group_member")]
            internal extern static IntPtr DisposeGroupMember(IntPtr entryHandle);
            [DllImport("libgkid.so", CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_group_member_count")]
            internal extern static int GetGroupMemberCount(IntPtr entryHandle);
        }
    }
}