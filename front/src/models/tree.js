export default {
    namespace: 'tree',
    state: {
        path: '',
        pathItems: [],
        items: []
    },
    effects: {
        async getTree ({ get }, { payload: { repositoriesName, repositoryName, branchName, path }, state }) {
            if (path === state.path) {
                return;
            }
            let result = await get (`/api/git/${repositoriesName}/${repositoryName}/${branchName}/tree${path}`);

            if (result.status === 200) {
                this.dispatch({ type: 'tree/reduce', payload: {
                    content: result.payload,
                    path,
                    repositoryName
                }});
            }
        },

        enterSubTree (method, { payload: { repositoriesName, repositoryName, branchName, path, type }, state }) {
            if (type === 'Tree') {
                window.location.hash = `/repository/${repositoriesName}/${repositoryName}/${branchName}/tree${path}`;
            }
            else if (type === 'Blob') {
                window.location.hash = `/repository/${repositoriesName}/${repositoryName}/${branchName}/blob${path}`;
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