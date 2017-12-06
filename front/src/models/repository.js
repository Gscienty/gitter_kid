export default {
    namespace: 'repository',
    state: {
        queryResult: [],
        singleDisplayType: '',
        branch: [],
        fs: {
            repository: '',
            flag: '',
            path: '',
            content: []
        },
        file: {
            repository: '',
            flag: '',
            path: '',
            content: ''
        }
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

        async tree({ get }, { payload: { repository, flag, path } }) {
            let result = await get(`/api/repository-fs/${repository}/${flag}/tree${path}`);
            this.dispatch({ type: 'repository/singleDisplayType', payload: 'tree' });
            if (result.status === 200) {
                this.dispatch({
                    type: 'repository/fs',
                    payload: {
                        repository,
                        flag,
                        path,
                        content: result.payload
                    }
                });
            }
            else {
                this.dispatch({ type: 'repository/fs', payload: null });
            }
        },

        async blob({ get }, { payload: { repository, flag, path } }) {
            let result = await get(`/api/repository-fs/${repository}/${flag}/blob${path}`);
            this.dispatch({ type: 'repository/singleDisplayType', payload: 'blob' });
            if (result.status === 200) {
                this.dispatch({
                    type: 'repository/file',
                    payload: {
                        repository,
                        flag,
                        path,
                        content: result.payload
                    }
                });
            }
            else {
                this.dispatch({ type: 'repository/file', payload: null });
            }
        },

        async repositoryBranch({ get }, { payload: { repository }}) {
            let result = await get(`/api/repository/${repository}/branch`);

            if (result.status === 200) {
                this.dispatch({ type: 'repository/branch', payload: result.payload });
            }
            else {
                this.dispatch({ type: 'repository/branch', payload: [] });
            }
        }
    },
    reduces: {
        queryResult: (state, payload) => ({ ...state, queryResult: payload }),
        fs: (state, payload) => ({ ...state, fs: payload }),
        file: (state, payload) => ({ ...state, file: payload }),
        singleDisplayType: (state, payload) => ({ ...state, singleDisplayType: payload }),
        branch: (state, payload) => ({ ...state, branch: payload })
    }
}