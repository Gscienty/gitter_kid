export default {
    namespace: 'branches',
    state: {
        name: '',
        branches: []
    },
    effects: {
        async getBranches({ get }, { payload: { repositoriesName, repositoryName } }) {
            let result = await get(`/api/git/${repositoriesName}/${repositoryName}`);

            if (result.status === 200) {
                this.dispatch({
                    type: 'branches/reduce',
                    payload: { name: repositoryName, branches: result.payload }
                });
            }
        }
    },
    reduces: {
        reduce(state, payload) { return payload; }
    }
}