export default {
    namespace: 'tree',
    state: {
        path: '',
        content: []
    },
    effects: {
        async getTree({ get }, { payload: { repositoriesName, repositoryName, branchName, path } }) {
            console.log(path);

            let result = await get(`/api/git/${repositoriesName}/${repositoryName}/${branchName}/tree${path}`);

            if (result.status === 200) {
                this.dispatch({
                    type: 'tree/reduce',
                    payload: { path: path, content: result.payload }
                });
            }
        }
    },
    reduces: {
        reduce(state, payload) { return payload; }
    }
}