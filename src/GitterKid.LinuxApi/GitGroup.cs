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

        public virtual GitGroupMembers Members
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

            public int Count { get; private set; }
            internal GitGroupMembers(IntPtr membersHandle)
            {
                this._membersHandle = membersHandle;
                this.Count = GitGroups.NativeMethod.GetGroupMemberCount(this._membersHandle);
            }

            IEnumerator<string> IEnumerable<string>.GetEnumerator() => new GitGroupMemberEnumerator(this._membersHandle, this.Count);

            IEnumerator IEnumerable.GetEnumerator() => new GitGroupMemberEnumerator(this._membersHandle, this.Count);
        }

        public class GitGroupMemberEnumerator : IEnumerator<string>
        {
            private IntPtr _membersHandle;
            private int _count;
            private bool _isFirst;

            string IEnumerator<string>.Current => this.GetCurrentGroupName();

            object IEnumerator.Current => this.GetCurrentGroupName();

            internal GitGroupMemberEnumerator(IntPtr membersHandle, int count)
            {
                this._membersHandle = membersHandle;
                this._isFirst = true;
                this._count = count;
            }
            private string GetCurrentGroupName()
            {
                if (this._count == 0)
                {
                    return string.Empty;
                }
                return Marshal.PtrToStringAnsi(GitGroups.NativeMethod.GetCurrentMemberName(this._membersHandle));    
            }

            void IDisposable.Dispose() { }

            bool IEnumerator.MoveNext()
            {
                if (this._count == 0)
                {
                    return false;
                }
                
                if (this._isFirst)
                {
                    this._isFirst = false;
                    return true;
                }
                else
                {
                    return GitGroups.NativeMethod.GroupMemberMoveNext(this._membersHandle) == 0;
                }
            }

            void IEnumerator.Reset()
            {
                GitGroups.NativeMethod.ResetMemberCursor(this._membersHandle);
                this._isFirst = true;
            }
        }
    }
}