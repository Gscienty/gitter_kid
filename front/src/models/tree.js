
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
        pathItems: [],
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
                this.dispatch({ type: 'tree/reduce', payload: {
                    path: currentPath,
                    content: result.payload,
                    repositoryName
                }});
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
                pathItems: (() => {
                    let treePath = payload.path;
                    if (treePath.endsWith('/')) {
                        treePath = treePath.substring (0, treePath.length - 1);
                    }
                    let pathItems = treePath.split('/');
                    pathItems[0] = { name: '', path: '' };
                    for (let i = 1; i < pathItems.length; i++) {
                        pathItems[i] = { name: pathItems[i], path: pathItems[i - 1].path + '/' + pathItems[i] };
                    }

                    pathItems[0].name = payload.repositoryName;
                    pathItems[0].path = '/';

                    return pathItems;
                })(),
                items: payload.content.map(item => ({
                    ...item,
                    path: payload.path + (payload.path.endsWith('/') ? '' : '/') + item.name + (item.type === 'Tree' ? '/' : '')
                }))
            };
        }
    }
}