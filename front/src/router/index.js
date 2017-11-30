import React, { Component } from 'react';
import { connect } from 'react-redux';

class Page extends Component {
    componentWillMount() {
        this.props.dispatch({
            type: 'category/get',
            payload: {
                type: 'single',
                id: '1'
            }
        })
    }
    render() {
        return <div>{ this.props.category.name }</div>;
    }
}

export default connect(
    ({ category }) => ({ category: category.single })
)(Page);