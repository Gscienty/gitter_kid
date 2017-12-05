export default {
    namespace: 'repository',
    state: {
        queryResult: [],

        focusName: 'unknow',
        focusFlag: 'unknow',
        focusFolder: []
    },
    effects: {
        async query({ get }, { payload: { keyword } }) {
            let result = await get(`/api/repository/query${keyword === undefined ? '' : `?keyword=${keyword}`}`);

            if (result.status === 200) {
                this.dispatch({ type: 'repository/queryResult', payload: result.payload });
            }
            else {
                this.dispatch({ type: 'repository/queryResult', payload: [] });
            }
        },

        async getFolder({ get }, { payload: { repositoryName, flag, path } }) {
            let result = await get(`/api/${repositoryName}/fs-${flag}/tree${path}`);

            if (result.status === 200) {
                this.dispatch({ type: 'repository/focusFolder', payload: result.payload });
            }
            else {
                this.dispatch({ type: 'repository/focusFolder', payload: null });
            }
        }
    },
    reduces: {
        queryResult: (state, payload) => ({ ...state, queryResult: payload }),
        focusName: (state, payload) => ({ ...state, focusName: payload }),
        focusFlag: (state, payload) => ({ ...state, focusFlag: payload }),
        folder: (state, payload) => ({ ...state, folder: payload }),
        focusFolder: (state, payload) => ({ ...state, focusFolder: payload })
    }
}