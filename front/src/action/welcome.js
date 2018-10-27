import React, { Component } from 'react';
import Nav from '../component/head'

class WelcomeAction extends Component {
    render() {
        return <div>
            <Nav isLogin={ true } username={ '张三' } />
        </div>;
    }
}

export default WelcomeAction;
