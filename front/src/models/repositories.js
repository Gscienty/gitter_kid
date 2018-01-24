export default {
    namespace: 'repositories',
    state: {
        name: 'default',
        repositories: []
    },
    effects: {
        async repositories({ get }, { payload: { name } }) {
            let result = await get(`/api/git/${name}`);

            if (result.status === 200) {
                this.dispatch({
                    type: 'repositories/reduce',
                    payload: { name, repositories: result.payload }
                });
            }
        }
    },
    reduces: {
        reduce(state, payload) { return payload; }
    }
}