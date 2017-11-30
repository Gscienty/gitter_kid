export default {
    namespace: 'category',
    state: {
        list: [],
        single: {}
    },
    effects: {
        async get({ get }, { type, id }) {
            if (type === 'single') {
                let result = await get(`/api/category/${id}`);
                if (result.status === 200) {
                    this.dispatch({ type: 'category/single', payload: result.payload });
                }
                else {
                    this.dispatch({ type: 'category/single', payload: { name: 'test' } });
                }
            }
        }
    },
    reduces: {
        list: (state, payload) => ({ ...state, list: payload }),
        single: (state, payload) => ({ ...state, single: payload })
    }
}