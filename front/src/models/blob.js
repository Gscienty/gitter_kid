export default {
    namespace: 'blob',
    state: {
        path: '',
        content: { content: '', length: '' }
    },
    effects: {
        async getBlob({ get }, { payload: { repositoriesName, repositoryName, branchName, path } }) {
            let result = await get(`/api/git/${repositoriesName}/${repositoryName}/${branchName}/blob${path}`);

            if (result.status === 200) {
                this.dispatch({
                    type: 'blob/reduce',
                    payload: { path, content: result.payload }
                });
            }
        }
    },
    reduces: {
        reduce(state, payload) { return payload; }
    }
}