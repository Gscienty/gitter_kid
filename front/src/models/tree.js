import pathTransferItems from '../util/pathTransferItems';

export default {
    namespace: 'tree',
    state: {
        path: '',
        pathItems: [],
        items: []
    },
    asyncEffects: {
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
        }
    },
    syncEffects: {
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
                pathItems: pathTransferItems (payload.repositoryName, payload.path),
                items: payload.content.map(item => ({
                    ...item,
                    path: payload.path + (payload.path.endsWith('/') ? '' : '/') + item.name + (item.type === 'Tree' ? '/' : '')
                }))
            };
        }
    }
}