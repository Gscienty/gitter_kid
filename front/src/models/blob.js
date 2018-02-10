import pathTransferItems from '../util/pathTransferItems';

export default {
    namespace: 'blob',
    state: {
        path: '',
        pathItems: [],
        name: '',
        content: ''
    },
    asyncEffects: {
        async getTextBlob({ get }, { payload: { repositoriesName, repositoryName, branchName, path } }) {
            let result = await get(
                `/api/git/${repositoriesName}/${repositoryName}/${branchName}/blob${path}`, {
                    'Content-Type': 'text/plain'
                }
            );

            if (result.status === 200) {
                this.dispatch({
                    type: 'blob/reduce',
                    payload: { path, repositoryName ,content: result.payload }
                });
            }
        }
    },
    reduces: {
        reduce(state, { path, content, repositoryName }) {
            return {
                path,
                content,
                pathItems: pathTransferItems (repositoryName, path),
                name: (() => {
                    let lastSpPosition = path.lastIndexOf ('/');
                    if (lastSpPosition === -1) {
                        return path;
                    }
                    return path.substring (lastSpPosition + 1);
                })()
            };
        }
    }
}