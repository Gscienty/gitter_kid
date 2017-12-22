using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Collections;

namespace GitterKid.LinuxApi
{
    public class GitGroup
    {
        private IntPtr _groupHandle;
        private string _groupName;
        private string _groupPassword;
        private int? _groupId;

        public virtual string GroupName
        {
            get
            {
                if (this._groupName == null)
                {
                    this._groupName = Marshal.PtrToStringAnsi(GitGroups.NativeMethod.GetGroupName(this._groupHandle));
                }
                return this._groupName;
            }
        }

        public virtual string GroupPassword
        {
            get
            {
                if (this._groupPassword == null)
                {
                    this._groupPassword = Marshal.PtrToStringAnsi(GitGroups.NativeMethod.GetGroupPassword(this._groupHandle));
                }
                return this._groupPassword;
            }
        }

        public virtual int GroupId
        {
            get
            {
                if (this._groupId.HasValue == false)
                {
                    this._groupId = GitGroups.NativeMethod.GetGroupId(this._groupHandle);
                }

                return this._groupId.Value;
            }
        }

        public GitGroup(IntPtr handle)
        {
            this._groupHandle = handle;
        }

        public class GitGroupMembers : IEnumerable<string>
        {
            private IntPtr _membersHandle;
            internal GitGroupMembers(IntPtr membersHandle)
            {
                this._membersHandle = membersHandle;
            }

            IEnumerator<string> IEnumerable<string>.GetEnumerator() => new GitGroupMemberEnumerator(this._membersHandle);

            IEnumerator IEnumerable.GetEnumerator() => new GitGroupMemberEnumerator(this._membersHandle);
        }

        public class GitGroupMemberEnumerator : IEnumerator<string>
        {
            private IntPtr _baseHandle;
            private int _cursor;
            string IEnumerator<string>.Current => this.GetCurrentGroupName();

            object IEnumerator.Current => this.GetCurrentGroupName();

            internal GitGroupMemberEnumerator(IntPtr baseHandle)
            {
                this._baseHandle = baseHandle;
                this._cursor = 0;
            }

            unsafe private string GetCurrentGroupName() => Marshal.PtrToStringAnsi(
                *(IntPtr *)(this._baseHandle + GitGroups.NativeMethod.GetMemberCursorSize() * this._cursor)
            );

            void IDisposable.Dispose()
            {
                
            }

            unsafe bool IEnumerator.MoveNext()
            {
                this._cursor++;

                return *(IntPtr *)(this._baseHandle + GitGroups.NativeMethod.GetMemberCursorSize() * this._cursor) != IntPtr.Zero;
            }

            void IEnumerator.Reset()
            {
                this._cursor = 0;
            }
        }
    }
}