import { createStore } from 'redux';

function __common_transfer_result(response) {
    return response.text()
        .then(jsonString => {
            if (jsonString.length === 0) {
                return { status: response.status };
            }
            else {
                return ({ status: response.status, payload: JSON.parse(jsonString) });
            }
        });
}

function __build_submit_body(entity, headers) {
    switch (headers['Content-Type']) {
        case 'application/json': return JSON.stringify(entity);
        case 'application/x-www-form-urlencoded':
            let body = '';
            for (let property in entity) {
                body += `${encodeURI(property)}=${encodeURI(entity[property])}&`;
            }
            return body.substring(0, body.length - 1);
        default: return entity;
    }
}

async function create(uri, entity, headers = { 'Content-Type': 'application/json' }) {
    return fetch(uri, {
        method: 'PUT',
        credentials: 'include',
        headers,
        body: __build_submit_body(entity, headers)
    }).then(response => __common_transfer_result(response));
}

async function get(uri, headers = { 'Content-Type': 'application/json' }) {
    return fetch(uri, {
        method: 'GET',
        credentials: 'include',
        headers
    }).then(response => __common_transfer_result(response));
}

async function update(uri, entity, headers = { 'Content-Type': 'application/json' }) {
    return fetch(uri, {
        method: 'POST',
        credentials: 'include',
        headers,
        body: __build_submit_body(entity, headers)
    }).then(response => __common_transfer_result(response));
}

async function remove(uri, headers = { 'Content-Type': 'application/json' }) {
    return fetch(uri, {
        method: 'DELETE',
        credentials: 'include',
        headers
    }).then(response => __common_transfer_result(response));
}


function _reducer(store, state, { type, payload }) {
    if (/^\w+\/\w+$/.test(type)) {
        let direct = type.split('/', 2);
        let namespace = direct[0];
        let action = direct[1];
        if (namespace in this._effects && action in this._effects[namespace]) {
            this._effects[namespace][action]
                .call(store, { create, update, remove, get }, { payload, state: state[namespace] });
        }
        else if (namespace in this._reduces && action in this._reduces[namespace]) {
            let next = this._reduces[namespace][action]
                .call(store, state[namespace], payload);
            let tempState = { ...state };
            tempState[namespace] = next;
            return tempState;
        }
    }

    return state;
}

export class StoreManager {
    _initializeStore = { }
    _effects = { }
    _reduces = { }

    register({ namespace, state, effects, reduces }) {
        // register effects
        this._effects[namespace] = effects;
        // register reduces
        this._reduces[namespace] = reduces;
        // register store
        this._initializeStore[namespace] = state;
    }

    build(submitMime) {
        var store = createStore(
            (state, action) => _reducer.call(this, store, state, action),
            this._initializeStore
        );
        return store;
    }
}