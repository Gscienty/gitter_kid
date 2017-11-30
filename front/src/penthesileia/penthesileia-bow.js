import React from 'react';
import ReactDOM from 'react-dom';
import { StoreManager } from './store';
import { Provider } from 'react-redux';
import { Route } from 'react-router-dom';


export default class PenthesileiaBow {
    _storeManager = new StoreManager();
    _pages = []

    addModel(model) {
        this._storeManager.register(model);
    }

    addPage(pageMeta) { this._pages.push(pageMeta); }

    run(dom, Router, submitMime = 'application/json') {
        ReactDOM.render(<Provider store={ this._storeManager.build(submitMime) }>
            <Router>
                <div>
                    { this._pages.map((page, key) => <Route key={ key } { ...page } />) }
                </div>
            </Router>
        </Provider>, dom);
    }
}