export default {
    namespace: 'repository',
    state: {
        list: []
    },
    effects: {
        async get({ get }, { payload: { keyword } }) {
            let result = await get(`/api/repository/query${keyword === undefined ? '' : `?keyword=${keyword}`}`);

            if (result.status === 200) {
                this.dispatch({ type: 'repository/list', payload: result.payload });
            }
            else {
                this.dispatch({ type: 'repository/list', payload: [] });
            }
        }
    },
    reduces: {
        list: (state, payload) => ({ ...state, list: payload })
    }
}