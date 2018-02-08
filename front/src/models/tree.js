
function __getTreePathFromHash () {
    let treeUriPosition = window.location.hash.indexOf ('/tree');
    let path = window.location.hash.substring (treeUriPosition + '/tree'.length);
    return path;
}

function __getPrefixHash () {
    return window.location.hash.split('/tree', 1)[0];
}

export default {
    namespace: 'tree',
    state: {
        path: '',
        items: []
    },
    effects: {
        async getTree ({ get }, { payload: { repositoriesName, repositoryName, branchName }, state: { path } }) {
            let currentPath = __getTreePathFromHash();
            if (currentPath === path) {
                return;
            }
            let result = await get (`/api/git/${repositoriesName}/${repositoryName}/${branchName}/tree${currentPath}`);

            if (result.status === 200) {
                this.dispatch({ type: 'tree/reduce', payload: { path: currentPath, content: result.payload }});
            }
        },

        enterSubTree (method, { payload: { path, type }, state }) {
            let prefix = __getPrefixHash ();
            if (type === 'Tree') {
                window.location.hash = prefix + '/tree' + path;
            }
            else if (type === 'Blob') {
                window.location.hash = prefix + '/blob' + path;
            }
        }
    },
    reduces: {
        reduce(state, payload) {
            return {
                path: payload.path,
                items: payload.content.map(item => ({
                    ...item,
                    path: payload.path + (payload.path.endsWith('/') ? '' : '/') + item.name + (item.type === 'Tree' ? '/' : '')
                }))
            };
        }
    }
}