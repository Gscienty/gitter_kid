using System;
using System.Runtime.InteropServices;

namespace GitterKid.LinuxApi
{
    public class GitUser
    {
        private IntPtr _entry;
        private string _gecos;
        private int? _userId;
        private int? _groupId;
        private string _userName;
        private string _homeDirectory;
        private string _shell;
        
        public virtual string Gecos
        {
            get
            {
                if (this._gecos == null)
                {
                    this._gecos = Marshal.PtrToStringAnsi(GitUsers.NativeMethod.GetPasswdGecos(this._entry));
                }
                return this._gecos;
            }
        }
        public int UserId
        {
            get
            {
                if (this._userId.HasValue == false)
                {
                    this._userId = GitUsers.NativeMethod.GetPasswdUserId(this._entry);
                }
                return this._userId.Value;
            }
        }
        public int GroupId
        {
            get
            {
                if (this._groupId.HasValue == false)
                {
                    this._groupId = GitUsers.NativeMethod.GetPasswdGroupId(this._entry);
                }
                return this._groupId.Value;
            }
        }
        public string UserName
        {
            get
            {
                if (this._userName == null)
                {
                    this._userName = Marshal.PtrToStringAnsi(GitUsers.NativeMethod.GetPasswdUserName(this._entry));
                }
                return this._userName;
            }
        }
        public string HomeDirectory
        {
            get
            {
                if (this._homeDirectory == null)
                {
                    this._homeDirectory = Marshal.PtrToStringAnsi(GitUsers.NativeMethod.GetPasswdHomeDirectory(this._entry));
                }
                return this._homeDirectory;
            }
        }
        public string Shell
        {
            get
            {
                if (this._shell == null)
                {
                    this._shell = Marshal.PtrToStringAnsi(GitUsers.NativeMethod.GetPasswdShell(this._entry));
                }
                return this._shell;
            }
        }


        public GitUser(IntPtr entry)
        {
            this._entry = entry;
        }
    }
}