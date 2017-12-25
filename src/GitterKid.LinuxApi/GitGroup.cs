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

        public virtual IEnumerable<string> Members
        {
            get
            {
                return new GitGroupMembers(GitGroups.NativeMethod.GetGroupMemberCursor(this._groupHandle));
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
            private IntPtr _memberHandle;
            string IEnumerator<string>.Current => this.GetCurrentGroupName();

            object IEnumerator.Current => this.GetCurrentGroupName();

            internal GitGroupMemberEnumerator(IntPtr memberHandle)
            {
                this._memberHandle = memberHandle;
            }
            private string GetCurrentGroupName()
            {
                return Marshal.PtrToStringAnsi(GitGroups.NativeMethod.GetCurrentMemberName(this._memberHandle));    
            }

            void IDisposable.Dispose() { }

            bool IEnumerator.MoveNext()
            {
                return GitGroups.NativeMethod.GroupMemberMoveNext(this._memberHandle) == 0;
            }

            void IEnumerator.Reset()
            {
                GitGroups.NativeMethod.ResetMemberCursor(this._memberHandle);
            }
        }
    }
}