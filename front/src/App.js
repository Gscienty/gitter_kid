import React, { Component } from 'react';
import { BrowserRouter, Route, Switch } from 'react-router-dom';

import WelcomeAction from './action/welcome';

class App extends Component {
    render() {
        return <BrowserRouter>
            <Switch>
                <Route path="/" component={ WelcomeAction } exact></Route>
            </Switch>
        </BrowserRouter>;
    }
}

export default App;
